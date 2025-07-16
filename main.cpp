#include <iostream>
#include <map>
#include <string>
#include <queue>

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
  std::map<Price, std::queue<Order>, std::greater<>> buyOrders;
  std::map<Price, std::queue<Order>> sellOrders;

  void addOrder(Order &order) {
    if (order.type == OrderType::buy) {
      buyOrders[order.price].push(order);
    } else {
      sellOrders[order.price].push(order);
    }
  }

  void printOrderBook() {
    std::cout << "BUY ORDERS:" << std::endl;
      for (const auto& : buyOrders) {
      int totalQuantity = 0;
      for (const auto& order : pair.second) {
        std::cout << "Price: " << order.price << " | " <<  std::endl;
        totalQuantity += order.quantity;
      }
    }

    for (const auto& pair : sellOrders) {

    }
  }

};

int main (int argc, char *argv[]) {
  Order order = {"A", OrderType::buy, 3.14, 3};
  return 0;
}
