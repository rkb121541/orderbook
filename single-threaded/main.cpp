#include <iostream>
#include <list>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>
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

struct Trade {
  Price price;
  Quantity quantity;
  Id buyId;
  Id sellId;
};

class OrderBook {
private:
  std::map<Price, std::list<Order>, std::greater<>> buyOrders;
  std::map<Price, std::list<Order>> sellOrders;
  std::unordered_map<Id, std::pair<Price, std::list<Order>::iterator>>
      orderById;
  std::vector<Trade> trades;

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
          trades.push_back({.price = sellOrder.price,
                            .quantity = matchQuantity,
                            .buyId = order.id,
                            .sellId = sellOrder.id});
          order.quantity -= matchQuantity;
          sellOrder.quantity -= matchQuantity;
          if (sellOrder.quantity == 0) {
            orderById.erase(sellOrder.id);
            sellQueue.pop_front();
          }
          if (sellQueue.empty())
            sellOrders.erase(bestAsk);
        } else {
          break;
        }
      }
      if (order.quantity > 0) {
        auto &orderList = buyOrders[order.price];
        orderList.push_back(order);
        auto it = std::prev(orderList.end());
        orderById[order.id] = {order.price, it};
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
          trades.push_back({.price = buyOrder.price,
                            .quantity = matchQuantity,
                            .buyId = buyOrder.id,
                            .sellId = order.id});
          order.quantity -= matchQuantity;
          buyOrder.quantity -= matchQuantity;
          if (buyOrder.quantity == 0) {
            orderById.erase(buyOrder.id);
            buyQueue.pop_front();
          }
          if (buyQueue.empty())
            buyOrders.erase(bestBid);
        } else {
          break;
        }
      }
      if (order.quantity > 0) {
        auto &orderList = sellOrders[order.price];
        orderList.push_back(order);
        auto it = std::prev(orderList.end());
        orderById[order.id] = {order.price, it};
        std::cout << "Sell order added to book (Id: " << order.id
                  << " Quantity: " << order.quantity
                  << " Price: " << order.price << ")" << std::endl;
      }
    }
  }

  bool cancelOrder(const Id &id) {
    auto it = orderById.find(id);
    if (it == orderById.end()) {
      std::cout << "Order " << id << " not found for cancellation" << std::endl;
      return false;
    }
    Price price = it->second.first;
    auto listIt = it->second.second;
    if (listIt->type == OrderType::buy) {
      auto &orderList = buyOrders[price];
      orderList.erase(listIt);
      if (orderList.empty())
        buyOrders.erase(price);
    } else {
      auto &orderList = sellOrders[price];
      orderList.erase(listIt);
      if (orderList.empty()) {
        sellOrders.erase(price);
      }
    }
    orderById.erase(it);
    std::cout << "Order " << id << " cancelled" << std::endl;
    return true;
  }

  bool modifyOrder(const Id &id, std::optional<Price> newPrice,
                   std::optional<Quantity> newQuantity) {
    auto it = orderById.find(id);
    if (it == orderById.end()) {
      std::cout << "Order " << id << " not found for modification" << std::endl;
      return false;
    }
    auto [oldPrice, listIt] = it->second;
    Order oldOrder = *listIt;
    OrderType type = oldOrder.type;
    auto &oldList =
        (type == OrderType::buy) ? buyOrders[oldPrice] : sellOrders[oldPrice];
    oldList.erase(listIt);
    if (oldList.empty()) {
      if (type == OrderType::buy) {
        buyOrders.erase(oldPrice);
      } else {
        sellOrders.erase(oldPrice);
      }
    }
    orderById.erase(it);
    Price updatedPrice = newPrice.value_or(oldOrder.price);
    Quantity updatedQuantity = newQuantity.value_or(oldOrder.quantity);
    Order updatedOrder = {.id = id,
                      .type = type,
                      .price = updatedPrice,
                      .quantity = updatedQuantity
    };
    auto &newList = (type == OrderType::buy) ? buyOrders[updatedPrice] : sellOrders[updatedPrice];
    newList.push_back(updatedOrder);
    auto newIt = std::prev(newList.end());
    orderById[id] = {updatedPrice, newIt};
    return true;
  }

  void printTopLevels(int depth) {
    std::cout << "TOP " << depth << " LEVELS:" << std::endl;
    std::cout << "  BIDS:" << std::endl;
    int count = 0;
    for (const auto &pair : buyOrders) {
      if (count >= depth)
        break;
      int totalQuantity = 0;
      for (const auto &order : pair.second) {
        totalQuantity += order.quantity;
      }
      std::cout << "    Price: " << pair.first
                << " | Quantity: " << totalQuantity << std::endl;
      count++;
    }
    for (int i = count; i < depth; i++) {
      std::cout << "    None" << std::endl;
    }
    std::cout << "  ASKS:" << std::endl;
    count = 0;
    for (const auto &pair : sellOrders) {
      if (count >= depth)
        break;
      int totalQuantity = 0;
      for (const auto &order : pair.second) {
        totalQuantity += order.quantity;
      }
      std::cout << "    Price: " << pair.first
                << " | Quantity: " << totalQuantity << std::endl;
      count++;
    }
    for (int i = count; i < depth; i++) {
      std::cout << "    None" << std::endl;
    }
  }

  void printOrderBook() {
    std::cout << "BUY ORDERS:" << std::endl;
    for (const auto &pair : buyOrders) {
      int totalQuantity = 0;
      std::vector<Id> ids;
      for (const auto &order : pair.second) {
        totalQuantity += order.quantity;
        ids.push_back(order.id);
      }
      std::cout << "  Price: " << pair.first << " | ";
      std::cout << "Quantity: " << totalQuantity << " | " << "Orders: ";
      std::cout << "[";
      for (size_t i = 0; i < ids.size(); i++) {
        std::cout << ids[i];
        if (i != ids.size() - 1)
          std::cout << ", ";
      }
      std::cout << "]" << std::endl;
    }
    std::cout << "SELL ORDERS:" << std::endl;
    for (const auto &pair : sellOrders) {
      int totalQuantity = 0;
      std::vector<Id> ids;
      for (const auto &order : pair.second) {
        totalQuantity += order.quantity;
        ids.push_back(order.id);
      }
      std::cout << "  Price: " << pair.first << " | ";
      std::cout << "Quantity: " << totalQuantity << " | " << "Orders: ";
      std::cout << "[";
      for (size_t i = 0; i < ids.size(); i++) {
        std::cout << ids[i];
        if (i != ids.size() - 1)
          std::cout << ", ";
      }
      std::cout << "]" << std::endl;
    }
  }
};

int main(int argc, char *argv[]) {
  OrderBook orderbook;

  Order s1 = {
      .id = "S1", .type = OrderType::sell, .price = 100.0, .quantity = 10};
  Order s2 = {
      .id = "S2", .type = OrderType::sell, .price = 101.0, .quantity = 15};
  Order s3 = {
      .id = "S3", .type = OrderType::sell, .price = 100.0, .quantity = 5};
  Order b1 = {
      .id = "B1", .type = OrderType::buy, .price = 99.0, .quantity = 20};
  Order b2 = {
      .id = "B2", .type = OrderType::buy, .price = 100.0, .quantity = 10};

  orderbook.addOrder(s1);
  orderbook.addOrder(s2);
  orderbook.addOrder(s3);
  orderbook.addOrder(b1);
  orderbook.addOrder(b2);
  orderbook.printOrderBook();

  orderbook.modifyOrder("B1", 100.0, std::nullopt);
  orderbook.modifyOrder("S1", std::nullopt, 8);
  orderbook.modifyOrder("S3", 102.0, 5);

  orderbook.cancelOrder("S2");
  orderbook.cancelOrder("S1");
  orderbook.cancelOrder("B1");

  orderbook.printOrderBook();

  orderbook.printTopLevels(3);

  return 0;
}
