#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

using namespace std;

class Card {
public:
	int suit;
	int num;

	Card() {}
	Card(int suit, int num) : suit(suit), num(num) {}
};

class Deck {
private:
	vector<Card> cards;
	int top_card;

public:
	Deck() {}
	Deck(int nSuit, int nNum) : top_card(0) {	// Here the user can specify a different number of suits with a different number of card per suit.
		for (int i = 0; i < nSuit; ++i) {
			for (int j = 0; j < nNum; ++j) {
				cards.push_back(Card(i, j));
			}
		}
	}

	void shuffle() {
		for (int i = 1; i < cards.size(); ++i) {
			int j = rand() % (i+1);

			Card card = cards[i];
			cards[i] = cards[j];
			cards[j] = card;
		}

		top_card = 0;	// After shuffling, the top card should be restarted from the beginning.
	}

	Card deal() {
		Card card = cards[top_card];
		++top_card;
		//cout << "dealed suit is: " << card.suit + 1 << "; num is: " << card.num + 1 << endl;
		return card;
	}

	bool empty() const {	// Only deal card when the deck is not empty.
		return top_card == cards.size();
	}

	void printCards() const {
		for (const auto& card : cards) {
			cout << "suit is: " << card.suit + 1 << "; num is: " << card.num + 1 << endl;
		}
		cout << endl;
	}
};

class Player {
public:
	vector<Card> hand;
	string name;
	int score;

	Player(const string name) : name(name), score(0) {}

	int totalValue() const {
		int value = 0;
		for (const auto& card : hand) {
			if (card.num == 0 || card.num >= 9) {	// A, J, Q, K have a value of 10.
				value += 10;
			}
			else {
				value += card.num + 1;
			}
		}
		return value;
	}

	void clearCards() {
		hand.clear();
	}

	int getScore() const {
		return score;
	}

	void incrementScore() {
		++score;
	}

	void getCard(Card card) {
		hand.push_back(card);
	}

	void discardCard() {
		int i = rand() % hand.size();	// Randomly pick a card to discard for later exchange
		hand.erase(hand.begin() + i);
	}

	string getName() const {
		return name;
	}

	void printHand() const {
		for (const auto& card : hand) {
			cout << "suit is: " << card.suit + 1 << "; num is: " << card.num + 1 << endl;
		}
	}
};

class Dealer {
private:
	Deck deck;
	vector<Player> players;
	int numCardsToDeal;
	int winner;


public:
	Dealer(const Deck& deck, const vector<Player>& players, int numCardsToDeal) : deck(deck), players(players), numCardsToDeal(numCardsToDeal) {
		if (this->players.size() < 2) throw std::invalid_argument("At least 2 players are needed.");
	}

	void play() {
		// At the beginning of each round, the players need to clear the cards from the previous round.
		for (auto& player : players) {
			player.clearCards();
		}
		deck.shuffle();
		//deck.printCards();
		dealCards();
		exchangeCards();
		updateScore();
	}

	bool isThereAWinner() {
		for (int i = 0; i < players.size(); ++i) {
			if (players[i].getScore() >= 10) {
				winner = i;
				return true;
			}
		}
		return false;
	}

	void printWinner() const {
		cout << "The final winner is " << players[winner].getName().c_str() << " with score " << players[winner].getScore() << endl;
	}

private:
	void dealCards() {
		for (auto& player : players) {
			for (int i = 0; i < numCardsToDeal; ++i) {
				if (deck.empty()) {
					throw std::runtime_error("Run out of cards in a deck.");
				}
				else {
					player.getCard(deck.deal());
				}
			}
		}
	}

	void exchangeCards() {
		for (auto& player : players) {
			// Pick a random number of cards to exchange: 0...numCardsToDeal
			int numCardsToExchange = rand() % (numCardsToDeal + 1);
			// Has to have 2 for loops here. One needs to discard all the cards first before taking new replacement cards.
			// Combining the for loops means a different rule and the new card after exchange might be discarded for exchange again.
			for (int i = 0; i < numCardsToExchange; ++i) {
				player.discardCard();
			}
			for (int i = 0; i < numCardsToExchange; ++i) {
				if (deck.empty()) {
					throw std::runtime_error("Run out of cards in a deck.");
				}
				else {
					player.getCard(deck.deal());
				}
			}
		}
	}

	void updateScore() {
		vector<int> values;
		for (auto& player : players) {
			values.push_back(player.totalValue());
			player.printHand();
			cout << player.getName().c_str() << "'s total value is: " << player.totalValue() << endl;
		}
		int highest = values[0];
		int winner = 0;
		int count = 1;

		for (int i = 1; i < values.size(); ++i) {
			if (values[i] > highest) {
				highest = values[i];
				winner = i;
				count = 1;
			}
			else if (values[i] == highest) {
				++count;
			}
		}

		// Only the single player with the highest value gets a score; if there is a tie, no one gets a score.
		if (count == 1) {
			players[winner].incrementScore();
			cout << "the winner is " << players[winner].getName().c_str() << " with score: " << players[winner].getScore() << endl;
		}
	}
};

int main() {
	srand(time(nullptr));

	Deck deck(4, 13);
	/*deck.printCards();
	deck.shuffle();
	deck.printCards();*/

	vector<Player> players;
	players.push_back(Player("John"));
	players.push_back(Player("Mary"));
	players.push_back(Player("Joe"));
	players.push_back(Player("Amy"));

	Dealer dealer(deck, players, 5);
	while (!dealer.isThereAWinner()) {
		dealer.play();
	}
	dealer.printWinner();

	return 0;
}