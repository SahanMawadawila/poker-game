#include<iostream>
#include<vector>
#include<random>
#include<algorithm>
#include<stack>
#include<map>
#include<fstream>
#include<string>
#include<iomanip>
using namespace std;

//globally open file
ofstream file("log.txt");

enum Suit {
	SPADE, DIAMOND, HEART, CLUB
};


enum Value {
	TWO = 2, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING, ACE
};

void giveCorrespondingSuitAndValue(string& suitName, string& valueString, int suit, int value) {
	switch (suit) {
	case 0:suitName = "SPADE"; break;
	case 1:suitName = "DIAMOND"; break;
	case 2:suitName = "HEART"; break;
	case 3:suitName = "CLUB"; break;
	}

	switch (value) {
	case 2:valueString = "TWO"; break;
	case 3:valueString = "THREE"; break;
	case 4:valueString = "FOUR"; break;
	case 5:valueString = "FIVE"; break;
	case 6:valueString = "SIX"; break;
	case 7:valueString = "SEVEN"; break;
	case 8:valueString = "EIGHT"; break;
	case 9:valueString = "NINE"; break;
	case 10:valueString = "TEN"; break;
	case 11:valueString = "JACK"; break;
	case 12:valueString = "QUEEN"; break;
	case 13:valueString = "KING"; break;
	case 14:valueString = "ACE"; break;
	}
}


class Cards {
private:
	int value;
	int suit;
public:
	Cards(int val_, int suit_) :value(val_), suit(suit_) {}
	int getValue() {
		return value;
	}
	int getSuit() {
		return suit;
	}
};

class Deck {
private:

public:
	vector<Cards> cardPack;
	Deck() {
		for (int x = Suit::SPADE; x <= Suit::CLUB; x++) {
			for (int y = Value::TWO; y <= Value::ACE; y++) {
				Cards c(y, x);
				cardPack.push_back(c);
			}
		}
	}

};

bool compareByValue(Cards& a, Cards& b) {
	return a.getValue() < b.getValue();
}

class Player {
private:
	int score;
	vector<Cards> hand;
	string name;
	int points;
	float probabilityPoints;
public:
	string type;
	Player(string name_) :name(name_) {
		score = 0;
		points = 0;
		probabilityPoints = 0;

	}
	void receiveCards(Cards& card) {
		hand.push_back(card);
	}
	vector<Cards> getCards() {
		return hand;
	}
	void removeCardsFromPlayer() {
		hand.clear();
	}
	void printCardsToLog() {
		file << "Player " << name << " cards:" << endl;
		for (size_t i = 0; i < hand.size(); ++i) {
			string suitName, valueString;
			giveCorrespondingSuitAndValue(suitName, valueString, hand[i].getSuit(), hand[i].getValue());
			file << suitName << " " << valueString;
			if (i < hand.size() - 1) {
				file << ",";
			}
		}
		file << endl << endl;
	}
	void sortHand() {
		sort(hand.begin(), hand.end(), compareByValue);
	}
	void increaseScore(int score_) {
		score += score_;
	}
	void printTypeToLog() {
		file << name << " :" << type << endl;
	}
	string getName() {
		return name;
	}
	int getScore() {
		return score;
	}
	void setScoreToZero() {
		score = 0;
	}
	void increasePoints(int pnts) {
		points += pnts;
	}
	int getPoints() {
		return points;
	}
	void increaseProbabilitypoints(int x) {
		probabilityPoints += x;
	}
	int getProbabilityPoints() {
		return probabilityPoints;
	}
};

class Dealer :public Player {
public:
	stack<Cards> suffledCardPack;
	Dealer() :Player("Dealer") {}
	void shuffle1(Deck& deck) {
		// Create a random number generator
		random_device rd;
		mt19937 g(rd());

		//shuffle to randomize the cardPack vector
		shuffle(deck.cardPack.begin(), deck.cardPack.end(), g);

		//remove if there are elements in stack
		while (!suffledCardPack.empty()) {
			suffledCardPack.pop();
		}
		//copyToStack
		for (Cards x : deck.cardPack) {
			suffledCardPack.push(x);
		}
	}
	void removeTwoCards() {
		suffledCardPack.pop();
		suffledCardPack.pop();

	}
	void distributeCardsToDealer() {
		for (int i = 0; i < 5; i++) {
			receiveCards(suffledCardPack.top());
			suffledCardPack.pop();
		}
	}
	void distributeCardsToPlayers(Player& player) {
		for (int i = 0; i < 5; i++) {
			player.receiveCards(suffledCardPack.top());
			suffledCardPack.pop();
		}
	}
	void showCards() {
		vector<Cards> hand2 = getCards();
		cout << "\nDealers Cards:" << endl;
		file << "\nDealers Cards:" << endl;
		for (size_t i = 0; i < hand2.size(); ++i) {
			string suitName, valueString;
			giveCorrespondingSuitAndValue(suitName, valueString, hand2[i].getSuit(), hand2[i].getValue());
			cout << suitName << " " << valueString;
			file << suitName << " " << valueString;


			// Print a comma and space if this is not the last element
			if (i < hand2.size() - 1) {
				cout << ", ";
				file << ", ";
			}
		}

		cout << endl << endl;
		file << endl << endl;

	}
	void printTypeToLog() {
		file << "Dealer  " << " :" << endl;
	}
};

class Rules {
public:
	bool isSameSuit(vector<Cards>& hand_) {
		bool result = hand_[0].getSuit() == hand_[1].getSuit() && hand_[1].getSuit() == hand_[2].getSuit()
			&& hand_[2].getSuit() == hand_[3].getSuit() && hand_[3].getSuit() == hand_[4].getSuit();
		return result;
	}

	bool areValuesSequential(vector<Cards>& cards) {

		for (size_t i = 0; i < cards.size() - 1; i++) {
			if (cards[0].getValue() == 14 && cards[1].getValue() == 2) {
				continue;
			}
			if (cards[i].getValue() + 1 != cards[i + 1].getValue()) {
				// If the next card's value is not one greater than the current card's value, it's not sequential
				return false;
			}
		}
		return true;
	}

	int areFourOfKind_or_isThreeOfKind(vector<Cards>& cards) {
		stack<int> values;
		values.push(cards[0].getValue());
		for (size_t i = 1; i < cards.size(); ++i) {
			if (cards[i].getValue() == values.top()) {
				values.push(cards[i].getValue());
			}
			else {
				values = stack<int>();
				values.push(cards[i].getValue());
			}
		}
		return values.size();
	}

	int isFullHouse_isTwoPairs_isOnePair(vector<Cards>& cards) {
		stack<int> values;
		values.push(cards[0].getValue());
		for (size_t i = 1; i < cards.size(); ++i) {
			if (cards[i].getValue() == values.top()) {
				continue;
			}
			values.push(cards[i].getValue());
		}
		return values.size();

	}
};


class PokerGame {
private:
	Dealer& dealer;
	vector<Player>& players;
	Rules r;
	vector<Player*> allPlayers;
	vector<Player*> nonWithDrewPlayers;
public:
	PokerGame(Dealer& dealer_, vector<Player>& players_) : dealer(dealer_), players(players_) {
	}
	void enterPlayers() {
		allPlayers.push_back(&dealer);
		for (Player& x : players) {
			allPlayers.push_back(&x);
		}
	}
	void playGame() {

		char choice = 'n';
		int counter = 0;
		for (Player* player : allPlayers) {
			vector<Cards> hand_ = player->getCards();

			if (counter != 0) {
				cout << "Does Player " << player->getName() << " withdraw from game round? (y/n) :";
				cin >> choice;
			}
			counter++;
			if (choice == 'N' || choice == 'n') {
				nonWithDrewPlayers.push_back(player);
				int maxScore = hand_[4].getValue();
				if (r.areValuesSequential(hand_) && r.isSameSuit(hand_)) {
					player->type = "Straight Flush";
					player->increaseScore(900 + maxScore);
				}
				else if (r.areFourOfKind_or_isThreeOfKind(hand_) == 4) {
					player->type = "Four Of Kind";
					player->increaseScore(800 + maxScore);
				}
				else if (r.isFullHouse_isTwoPairs_isOnePair(hand_) == 2) {
					player->type = "Full House";
					player->increaseScore(700 + maxScore);
				}
				else if (r.isSameSuit(hand_)) {
					player->type = "Flush";
					player->increaseScore(600 + maxScore);
				}
				else if (r.areValuesSequential(hand_)) {
					player->type = "straight";
					player->increaseScore(500 + maxScore);
				}
				else if (r.areFourOfKind_or_isThreeOfKind(hand_) == 3) {
					player->type = "Three Of Kind";
					player->increaseScore(400 + maxScore);
				}
				else if (r.isFullHouse_isTwoPairs_isOnePair(hand_) == 3) {
					player->type = "Tow Pairs";
					player->increaseScore(300 + maxScore);
				}
				else if (r.isFullHouse_isTwoPairs_isOnePair(hand_) == 4) {
					player->type = "One Pair";
					player->increaseScore(200 + maxScore);
				}
				else {
					player->type = "High Card";
					player->increaseScore(100 + maxScore);
				}
			}
			
			else
			{
				player->type = "withdrew from the round ";
				player->increaseScore(0);
				player->increaseProbabilitypoints(1);
			}


		}
	}
	void selectWinner() {
		vector<int> score;
		stack<int> maximumScore;
		for (Player* player : nonWithDrewPlayers) {
			score.push_back(player->getScore());
		}
		maximumScore.push(score[0]);
		for (size_t i = 1; i < score.size(); i++) {
			if (score[i] >= maximumScore.top()) {
				if (score[i] > maximumScore.top()) {
					maximumScore = stack<int>();
					maximumScore.push(score[i]);
				}
				else {
					maximumScore.push(score[i]);
				}
			}
		}
		if (maximumScore.size() == 1) {
			for (Player* player : nonWithDrewPlayers) {
				if (maximumScore.top() == player->getScore()) {
					cout << "\n" << player->getName() << " is the winner of this round." << endl;
					file << "\n" << player->getName() << " is the winner of this round." << endl;
					if (player->getName() != "Dealer") {
						player->increasePoints(10);
						player->increaseProbabilitypoints(10);
					}
				}
				else if (player->getName() != "Dealer") {
					player->increasePoints(-1);
				}
			}
		}
		else {
			int count3 = 3;
			vector<Player*> temp;
			for (Player* player : nonWithDrewPlayers) {
				if (maximumScore.top() == player->getScore()) {
					temp.push_back(player);
				}
			}
			while (maximumScore.size() != 1) {
				maximumScore = stack<int>();
				for (size_t i = 0; i < temp.size(); i++) {
					temp[i]->increaseScore(temp[i]->getCards()[count3].getValue());
				}
				maximumScore.push(temp[0]->getScore());
				for (size_t i = 1; i < temp.size(); i++) {
					if (maximumScore.top() <= temp[i]->getScore()) {
						if (maximumScore.top() < temp[i]->getScore()) {
							maximumScore = stack<int>();
							maximumScore.push(temp[i]->getScore());
						}
						else {
							maximumScore.push(temp[i]->getScore());
						}
					}
				}
				temp.clear();
				for (Player* player : nonWithDrewPlayers) {
					if (maximumScore.top() == player->getScore()) {
						temp.push_back(player);
					}
				}
				count3--;
			}
			for (Player* player : nonWithDrewPlayers) {
				if (maximumScore.top() == player->getScore()) {
					cout << "\n" << player->getName() << " is the winner of this round." << endl;
					file << "\n" << player->getName() << " is the winner of this round." << endl;
					if (player->getName() != "Dealer") {
						player->increasePoints(10);
						player->increaseProbabilitypoints(10);
					}
				}
				else if (player->getName() != "Dealer") {
					player->increasePoints(-1);
				}

			}
			temp.clear();
		}
		maximumScore = stack<int>();
	}
	void setnonWithDrewPlayersToZero() {
		nonWithDrewPlayers.clear();
	}
	float calculateWinningProbability(Player* player) {
		float totalScore = 0;
		for (Player* x : allPlayers) {
			totalScore += x->getProbabilityPoints();
		}
		float winningProbabilityOfThisRound = player->getProbabilityPoints() / totalScore * 100;
		return winningProbabilityOfThisRound
			;
	}

	void printScoreCard(int round) {
		cout << "\n" << "\t      " << "-------- SCORECARD AFTER " << round << "st ROUND --------" << endl;
		file << "\n" << "\t      " << "-------- SCORECARD AFTER " << round << "st ROUND --------" << endl;
		cout << "Player " << "                    " << "Score " << "                    " << "Probability of winning the game" << endl;
		file << "Player " << "                    " << "Score " << "                    " << "Probability of winning the game" << endl;
		for (size_t i = 0; i < allPlayers.size(); i++) {
			cout << allPlayers[i]->getName() << "                    " << allPlayers[i]->getPoints() << "                                  " << fixed << setprecision(2) << calculateWinningProbability(allPlayers[i]) << endl;
			file << allPlayers[i]->getName() << "                    " << allPlayers[i]->getPoints() << "                                  " << fixed << setprecision(2) << calculateWinningProbability(allPlayers[i]) << endl;
		}

	}

	void selectGameWinnerAtEnd(int rounds) {
		stack<Player> theWinner;
		theWinner.push(players[0]);
		for (size_t i = 1; i < players.size(); i++) {
			if (theWinner.top().getPoints() <= players[i].getPoints()) {
				if (theWinner.top().getPoints() < players[i].getPoints()) {
					theWinner = stack<Player>();
					theWinner.push(players[i]);
				}
				else {
					theWinner.push(players[i]);
				}
			}
		}
		if (theWinner.size() == 1) {
			cout << "\nTHE WINNER OF WHOLE GAME AFTER " << rounds << " ROUNDS IS " << theWinner.top().getName() << endl;
			file << "\nTHE WINNER OF WHOLE GAME AFTER " << rounds << " ROUNDS IS " << theWinner.top().getName() << endl;
		}
		else {
			cout << "\nTHE WINNERS OF WHOLE GAME AFTER " << rounds << " ROUNDS ARE ";
			file << "\nTHE WINNERS OF WHOLE GAME AFTER " << rounds << " ROUNDS ARE ";
			while (theWinner.size() >= 1) {
				cout << theWinner.top().getName() << " ,";
				file << theWinner.top().getName() << " ,";

				theWinner.pop();
			}
			cout << endl;

		}
	}
};


void gettingUserinputs(int* noOfPlayers, int* rounds) {
	bool x = true;
	do {
		if (x) {
			cout << "Enter the num of players without dealer(less than 10):";
			cin >> *noOfPlayers;
			x = false;
		}
		else {
			cout << "Invalid Input!..Please enter the num of players without dealer(less than 10):";
			cin >> *noOfPlayers;
		}

	} while (*noOfPlayers >= 10);

	cout << "Enter game rounds:";
	cin >> *rounds;
}
int main() {
	cout << "............WELCOME TO THE POKER GAME SIMULATION............" << endl << endl;
	int noOfPlayers, rounds;
	gettingUserinputs(&noOfPlayers, &rounds);

	//creating player objects
	vector<Player> players;
	for (int i = 1; i <= noOfPlayers; i++) {
		Player p("Player " + to_string(i));
		players.push_back(p);
	}

	//dealer object
	Dealer dealer;

	//deck Object
	Deck deck;


	//creating game object
	PokerGame game(dealer, players);

	//enter players and dealer objcets to the game
	game.enterPlayers();

	for (int i = 1; i <= rounds; i++) {
		file << "---------------------------------------------------------------------" << endl;
		file << "Round " << i << endl;
		file << "---------------------------------------------------------------------" << endl;
		cout << "---------------------------------------------------------------------" << endl;
		cout << "Round " << i << endl;
		cout << "---------------------------------------------------------------------" << endl;

		//functions of dealer(shuffle and distribute)
		dealer.shuffle1(deck);
		dealer.removeTwoCards();
		dealer.distributeCardsToDealer();
		for (Player& x : players) {
			dealer.distributeCardsToPlayers(x);
		}
		dealer.sortHand();
		dealer.showCards();

		//print players cards to log
		for (Player& x : players) {
			x.sortHand();
			x.printCardsToLog();
		}


		//playgame
		game.playGame();

		//type
		dealer.printTypeToLog();
		for (Player& x : players) {
			x.printTypeToLog();
		}
		//printWinner
		game.selectWinner();

		game.setnonWithDrewPlayersToZero();

		//print scorecard
		game.printScoreCard(i);

		//score to zero
		dealer.setScoreToZero();
		for (Player& x : players) {
			x.setScoreToZero();
		}

		//after a round removing all cards 
		dealer.removeCardsFromPlayer();
		for (Player& x : players) {
			x.removeCardsFromPlayer();
		}

	}
	game.selectGameWinnerAtEnd(rounds);
	file.close();
	return 0;
}