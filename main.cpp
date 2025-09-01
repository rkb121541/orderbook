#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <vector>

enum class OrderType { buy, sell };

using Id = std::string;
using Price = double;
using Quantity = int;

struct Order {
  Id id;
  OrderType type;
  Price price;
  Quantity quantity;
};

class OrderBook {
private:
  std::map<Price, std::queue<Order>, std::greater<>> buyOrders;
  std::map<Price, std::queue<Order>> sellOrders;

public:
  void addOrder(Order &order) {
    if (order.type == OrderType::buy) {
      while (!sellOrders.empty() && order.quantity > 0) {
        auto bestAsk = sellOrders.begin();
        if (bestAsk->first <= order.price) {
          auto &sellQueue = bestAsk->second;
          Order &sellOrder = sellQueue.front();
          int matchQuantity = std::min(order.quantity, sellOrder.quantity);
          std::cout << "TRADE: " << matchQuantity << " @ " << sellOrder.price
                    << " (BUY: " << order.id << ", SELL: " << sellOrder.id
                    << ")" << std::endl;
          order.quantity -= matchQuantity;
          sellOrder.quantity -= matchQuantity;
          if (sellOrder.quantity == 0)
            sellQueue.pop();
          if (sellQueue.empty())
            sellOrders.erase(bestAsk);
        } else {
          break;
        }
      }
      if (order.quantity > 0) {
        buyOrders[order.price].push(order);
        std::cout << "Buy order added to book (Id: " << order.id
                  << " Quantity: " << order.quantity
                  << " Price: " << order.price << ")" << std::endl;
      }
    } else {
      while (!buyOrders.empty() && order.quantity > 0) {
        auto bestBid = buyOrders.begin();
        if (bestBid->first >= order.price) {
          auto &buyQueue = bestBid->second;
          Order &buyOrder = buyQueue.front();
          int matchQuantity = std::min(order.quantity, buyOrder.quantity);
          std::cout << "TRADE: " << matchQuantity << " @ " << buyOrder.price
                    << " (BUY: " << buyOrder.id << ", SELL: " << order.id << ")"
                    << std::endl;
          order.quantity -= matchQuantity;
          buyOrder.quantity -= matchQuantity;
          if (buyOrder.quantity == 0)
            buyQueue.pop();
          if (buyQueue.empty())
            buyOrders.erase(bestBid);
        } else {
          break;
        }
      }
      if (order.quantity > 0) {
        sellOrders[order.price].push(order);
        std::cout << "Sell order added to book (Id: " << order.id
                  << " Quantity: " << order.quantity
                  << " Price: " << order.price << ")" << std::endl;
      }
    }
  }

  void printOrderBook() {
    std::cout << "BUY ORDERS:" << std::endl;
    for (const auto &priceToQueue : buyOrders) {
      int totalQuantity = 0;
      std::vector<Id> ids;
      std::queue<Order> copyQueue = priceToQueue.second;
      std::cout << "Price: " << priceToQueue.first << " | ";
      while (!copyQueue.empty()) {
        Order copyOrder = copyQueue.front();
        totalQuantity += copyOrder.quantity;
        ids.push_back(copyOrder.id);
        copyQueue.pop();
      }
      std::cout << "Quantity: " << totalQuantity << " | " << "Orders: ";
      std::cout << "[";
      for (size_t i = 0; i < ids.size(); i++) {
        if (i == ids.size() - 1) {
          std::cout << ids[i] << "]" << std::endl;
        } else {
          std::cout << ids[i] << ", ";
        }
      }
    }
    std::cout << "SELL ORDERS:" << std::endl;
    for (const auto &priceToQueue : sellOrders) {
      int totalQuantity = 0;
      std::vector<Id> ids;
      std::queue<Order> copyQueue = priceToQueue.second;
      std::cout << "Price: " << priceToQueue.first << " | ";
      while (!copyQueue.empty()) {
        Order copyOrder = copyQueue.front();
        totalQuantity += copyOrder.quantity;
        ids.push_back(copyOrder.id);
        copyQueue.pop();
      }
      std::cout << "Quantity: " << totalQuantity << " | " << "Orders: ";
      std::cout << "[";
      for (size_t i = 0; i < ids.size(); i++) {
        if (i == ids.size() - 1) {
          std::cout << ids[i] << "]" << std::endl;
        } else {
          std::cout << ids[i] << ", ";
        }
      }
    }
  }
};

int main(int argc, char *argv[]) {
  OrderBook orderbook;

  Order sell1 = {"S1", OrderType::sell, 99.0, 5};
  Order sell2 = {"S2", OrderType::sell, 100.0, 10};
  Order buy = {"B1", OrderType::buy, 100.0, 12};

  orderbook.addOrder(sell1);
  orderbook.printOrderBook();
  orderbook.addOrder(sell2);
  orderbook.printOrderBook();
  orderbook.addOrder(buy);
  orderbook.printOrderBook();

  return 0;
}
