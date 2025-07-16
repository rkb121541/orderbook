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
      buyOrders[order.price].push(order);
    } else {
      sellOrders[order.price].push(order);
    }
  }

  void printOrderBook() {
    std::cout << "BUY ORDERS:" << std::endl;
    for (const auto &priceToQueue : buyOrders) {
      int totalQuantity = 0;
      std::vector<Id> orders;
      std::queue<Order> copyQueue = priceToQueue.second;
      std::cout << "Price: " << priceToQueue.first << " | ";
      while (!copyQueue.empty()) {
        Order copyOrder = copyQueue.front();
        totalQuantity += copyOrder.quantity;
        orders.push_back(copyOrder.id);
        copyQueue.pop();
      }
      std::cout << "Quantity: " << totalQuantity << " | " << "Orders: ";
      std::cout << "[";
      for (size_t i = 0; i < orders.size(); i++) {
        if (i == orders.size() - 1) {
          std::cout << orders[i] << "]" << std::endl;
        } else {
          std::cout << orders[i] << ", ";
        }
      }
    }
    std::cout << "SELL ORDERS:" << std::endl;
    // for (const auto &priceToQueue : sellOrders) {
    // }
  }
};

int main(int argc, char *argv[]) {
  OrderBook orderbook;

  Order order1 = {"A1", OrderType::buy, 100.5, 10};
  Order order2 = {"A2", OrderType::buy, 100.5, 20};
  Order order3 = {"A3", OrderType::buy, 99.0, 15};

  // Order order4 = {"B1", OrderType::sell, 101.0, 5};
  // Order order5 = {"B2", OrderType::sell, 102.5, 25};

  orderbook.addOrder(order1);
  orderbook.addOrder(order2);
  orderbook.addOrder(order3);
  // orderbook.addOrder(order4);
  // orderbook.addOrder(order5);

  orderbook.printOrderBook();

  return 0;
}
