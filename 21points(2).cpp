#include <iostream>
#include <string>
#include <list>
#include <vector>  
#include <algorithm>
#include <random>
#include <chrono>
#include <iomanip>
#include <windows.h>

using namespace std;

template<class T>
class Base {
public:

	friend ostream& operator<<(ostream& os, T& base) {
		os << base.str();
		return os;
	}
	virtual string str() { return ""; };
};

class CardType :public Base<CardType> {
public:
	enum Type {
		Spade = 0,
		Heart = 1,
		Club = 2,
		Diamond = 3
	};
	CardType(int cardType) : cardType(cardType) {}

	virtual string str() {
		static string strs[] = { "Spade","Heart", "Club" , "Diamond" };
		return strs[cardType];
	}
protected:
	const int cardType;
};

class Card :public Base<Card> {
public:
	Card(int cardId) :cardId(cardId), hide(false) {}
	Card& operator=(const Card& card) {
		cardId = card.cardId;
		hide = card.hide;
		return *this;
	}
	CardType getCardType() {
		return CardType((cardId - 1) / 13);
	}
	string getCardPosfix() {
		int idx = getCardIdx();
		if (idx >= 1 && idx <= 10) {
			return to_string(idx);
		}
		if (idx == 11) return "J";
		if (idx == 12) return "Q";
		if (idx == 13) return "K";
		return "";
	}
	int getCardIdx() {
		int idx = (cardId - 1) % 13 + 1;
		return idx;
	}
	int getCardValue() {
		int idx = getCardIdx();
		return idx > 10 ? 10 : idx;
	}
	void setHide(bool h) {
		hide = h;
	}
	bool isHided() { return hide; }
	virtual string str() {
		if (hide) return "*";
		return getCardType().str() + "<" + getCardPosfix() + ">";
	}
protected:
	// 1 - 52
	// Spade	1,2,3,4,5,6,7,8,9,10,J(11),Q(12),K(13)
	// Heart	+13
	// Club		+26
	// Diamond	+39
	int cardId;
	bool hide;
};


class Deck :public Base<Deck> {
public:
	Deck(int num = 2) : deckNum(num) { 
		reset(); 
	}
	void reset() {
		cards.clear();
		for (int i = 0; i < deckNum; i++) {
			for (int id = 1; id <= 52; id++) {
				cards.push_back(Card(id));
			}
		}
		shuffle();
	}
	void shuffle() {
		auto seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::shuffle(cards.begin(), cards.end(), std::default_random_engine((unsigned int)seed));
	}
	Card popOne() {
		Card card = cards.back();
		cards.pop_back();
		return card;
	}
protected:
	int deckNum;
	vector<Card> cards;
};

class Player :public Base<Player> {
	static int PlayerIDCount;
public:
	Player(float money = 0.0) :money(money), moneyBetted(0), lost(false){
		pid = PlayerIDCount;
		PlayerIDCount += 1;
	}
	bool addBet(float m) {
		if (m < 0) {
			return false;
		}
		if (moneyBetted + m > money) {
			return false;
		}
		moneyBetted += m;
		money -= m;
		return true;
	}
	bool isLost() {
		return lost;
	}
	void setLost() {
		lost = true;
	}
	void clearBet() {
		moneyBetted = 0;
	}
	void addMoney(float m) {
		money += m;
	}
	void addCard(Card card) {
		handCards.push_back(card);
	}
	virtual string str() {
		return "Player[" + to_string(pid) + "]";
	}
	float getMoney() { return money; }
	float getMoneyBetted() { return moneyBetted; }
	int calcCardsValue() {
		int total = 0;
		for (auto& c : handCards) {
			if (c.getCardValue() != 1) {
				total += c.getCardValue();
			}
		}
		for (auto& c : handCards) {
			if (c.getCardValue() == 1) {
				if ((total + 10) < 21 ){
					total += 10;
				} else {
					total += 1;
				}
			}
		}
		return total;
	}
	bool bursted() {
		return calcCardsValue() > 21;
	}
	void printDetail() {
		printBaseInfo();
		if (handCards.size() > 0) {
			printCards();
		}
	}
	virtual void printBaseInfo() {
		cout << setw(15) << *this << " money:" << setw(6) << getMoney();
		if (getMoneyBetted()) {
			cout << " betted:" << setw(6) << getMoneyBetted() << endl;
		}
		else {
			cout << endl;
		}
	}
	virtual void printCards() {
		cout << setw(15) << *this << " cards:";
		for (int i = 0; i < handCards.size(); i++) {
			Card& card = handCards[i];
			if (i == handCards.size() - 1) {
				cout << setw(12) << card << " = ";
			}
			else {
				cout << setw(12) << card << " + ";
			}
		}
		cout << calcCardsValue() << endl;
	}
	void clearHandCards() {
		handCards.clear();
	}
protected:
	vector<Card> handCards;
	float money;
	float moneyBetted;
	int pid;
	bool lost;
};
int Player::PlayerIDCount = 0;

class Dealer :public Player {
public:
	void showAllCards() {
		for (Card& card : handCards) {
			card.setHide(false);
		}
	}
	bool hasHidedCards() {
		for (Card& card : handCards) {
			if (card.isHided()) {
				return true;
			}
		}
		return false;
	}
	virtual void printBaseInfo() {
		cout << setw(15) << *this << " money:" << setw(6) << getMoney() << endl;
	}
	virtual void printCards() {
		cout << setw(15) << *this << " cards:";
		for (int i = 0; i < handCards.size(); i++) {
			Card& card = handCards[i];
			if (i == handCards.size() - 1) {
				if (card.isHided()) {
					cout << setw(2) << card;
				} else {
					cout << setw(12) << card << " = ";
				}
			}
			else {
				cout << setw(12) << card << " + ";
			}
		}
		if (hasHidedCards())
			cout << endl;
		else
			cout << calcCardsValue() << endl;
	}
	virtual string str() {
		return "Dealer";
	}
};


class GameState :public Base<GameState> {
public:
	enum State {
		Init,
		Bet,
		Deal,
		NewCard,
		RoundEnd,
		GameEnd
	};
	GameState(State state) : state(state) {}
	GameState& operator=(const GameState& s) {
		state = s.state;
		return *this;
	}
	bool is(State s) {
		return state == s;
	}
	virtual string str() const {
		static string strs[] = { "Init", "Bet", "Deal", "NewCard", "RoundEnd", "GameEnd" };
		return "<" + strs[state] + ">";
	}
	operator string() const {
		return str();
	}
protected:
	State state;
};


class Game :public Base<Game> {
public:
	Game() :state(GameState::Init) {}
	void Init(float dealerInitMoney, int playerNum, float playerInitMoney, int threshold) {
		for (int i = 0; i < playerNum; i++) {
			players.push_back(Player(playerInitMoney));
		}
		dealer.addMoney(dealerInitMoney);
		dealerThreshold = threshold;
	}
	void play() {
		while (1) {
			processCurrentState();
			if (state.is(GameState::GameEnd)){
				break;
			}
		}
	}
	void enterState(GameState newState) {
		cout << endl;
		info("enter state " + string(newState));
		state = newState;
		printGameState();
	}
	void processCurrentState() {
		if (state.is(GameState::Init)) {
			enterState(GameState::Bet);
		}
		else if (state.is(GameState::Bet)) {
			for (Player& p : players) {
				if (p.isLost()) {
					continue;
				}
				bool ok = false;
				do {
					cout << p << " please input bet:" << endl;
					float moneyBet = 0.0;
					cin >> moneyBet;
					ok = p.addBet(moneyBet);
					if (!ok) error("wrong input");

				} while (ok == false);
			}
			enterState(GameState::Deal);
		}
		else if (state.is(GameState::Deal)) {
			cout << "deal begin" << endl;
			for (int i = 0; i < 2; i++) {
				for (Player& p : players) {
					if (p.isLost()) {
						continue;
					}
					dealCardTo(p);
				}
				dealCardTo(dealer, i == 1);
			}
			cout << "deal finish" << endl;
			enterState(GameState::NewCard);
		}
		else if (state.is(GameState::NewCard)) {
			for (Player& p : players) {
				if (p.isLost()) {
					continue;
				}
				bool ret = false;
				do {
					cout << p << " please choose option: 1 = NewCard, 2 = Stand :" << endl;
					int option = 0;
					cin >> option;
					ret = false;
					if (option == 1) {
						dealCardTo(p);
						p.printCards();
						if (p.bursted()) {
							cout << setw(15) << p << " bursted" << endl;
							ret = true;
						}
					}
					else if (option == 2) {
						ret = true;
						// do nothing
					}
					else {
						error("wrong input");
					}
				} while (ret == false);
			}
			dealer.showAllCards();
			enterState(GameState::RoundEnd);
		}
		else if (state.is(GameState::RoundEnd)) {
			while (dealer.calcCardsValue() < dealerThreshold) {
				dealCardTo(dealer);
				dealer.printCards();
				if (dealer.bursted()) {
					cout << setw(15) << dealer << " bursted" << endl;
				}
			}
			cout << endl;

			cout << "round result:" << endl;
			cout << setw(12) << dealer << " score:" << dealer.calcCardsValue() << endl;
			for (auto& p : players) {
				if (p.isLost()) {
					continue;
				}
				cout << setw(12) << p << " score:" << p.calcCardsValue() << endl;
			}
			cout << endl;

			for (auto& p : players) {
				if(p.isLost()) {
					continue;
				}
				float m = p.getMoneyBetted();
				p.clearBet();
				if (dealer.calcCardsValue() == p.calcCardsValue()) {
					p.addMoney(m);
					cout << setw(12) << p << " return money:" << m << endl;
				}
				else if (dealer.bursted() || (!p.bursted() && p.calcCardsValue() > dealer.calcCardsValue())) {
					p.addMoney(m * 2.0f);
					dealer.addMoney(-m);
					cout << setw(12) << p << " win money:" << m * 2.0 << endl;
					cout << setw(12) << dealer << " lose money:" << m << endl;
				}
				else {
					dealer.addMoney(m);
					cout << setw(12) << dealer << " win money:" << m << endl;
					cout << setw(12) << p << " lose money:" << m << endl;
				}
			}

			cout << "round end." << endl << endl;

			deck.reset();
			dealer.clearHandCards();
			for (auto& p : players) {
				p.clearHandCards();
			}
			if (dealer.getMoney() <= 0) {
				enterState(GameState::GameEnd);
			}
			else {
				bool allHasNoMoney = true;
				for (auto& p : players) {
					if (p.getMoney() > 0) {
						allHasNoMoney = false;
						break;
					}
					else {
						p.setLost();
					}
				}
				if (allHasNoMoney) {
					enterState(GameState::GameEnd);
				}
				else {
					cout << "press any key to continue:" << endl;
					string anyKey;
					cin >> anyKey;
					enterState(GameState::Bet);
				}
			}
		}
	}
	void dealCardTo(Player& p, bool hide=false) {
		Card card = deck.popOne();
		if (hide) card.setHide(true);
		p.addCard(card);
		cout << "deal card " << setw(12) << card.str() << " to " << p.str() << endl;
	}
	void printGameState() {
		dealer.printDetail();
		for (auto& p : players) {
			p.printDetail();
		}
		cout << endl;
	}
	void error(const string err) {
		cout << "Error: " << err << endl;
	}
	void info(const string info) {
		cout << "Info: " << info << endl;
	}
	virtual string str() {
		return "Game";
	}
protected:
	list<Player> players;
	Dealer dealer;
	GameState state;
	Deck deck;
	int dealerThreshold;
};

int main()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED |FOREGROUND_BLUE |BACKGROUND_BLUE);
	cout<<"欢迎来到21点！" ; 
	Game game;
	game.Init(400, 1, 200, 17);
	game.play();
	cout<<"谢谢您的参与！您的选择是明智的，赌博有碍社会和谐和家庭健康，希望您远离赌博！"; 
	return 0;
}

