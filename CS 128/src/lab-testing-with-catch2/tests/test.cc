#ifndef CATCH_CONFIG_MAIN
#define CATCH_CONFIG_MAIN
#endif
#include <catch/catch.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "sandwich.hpp"

using std::string;
using std::vector;

// A helper method for you to visualize what the contents of a vector are. You
// are welcome to modify this method or create additional helper methods.
//
// Example usage:
//  vector<string> my_vec;
//  my_vec.push_back("hello");
//  my_vec.push_back("world"):
//  PrintVector(my_vec);
// Prints:
//  [hello, world]
void PrintVector(const vector<string>& vec) {
  std::cout << "[";
  bool first = true;
  for (const auto& it : vec) {
    if (!first) {
      std::cout << ", ";
    } else {
      first = false;
    }

    std::cout << it;
  }

  std::cout << "]" << std::endl;
}

TEST_CASE("Sandwich::AddTopping test", "[AddTopping]") {
  // your tests for Sandwich::AddTopping here
  SECTION("There shouldn't be a duplicate cheese") {
    Sandwich cheese_test;
    cheese_test.AddTopping("lettuce");
    cheese_test.AddTopping("cheese");
    cheese_test.AddTopping("tomato");
    cheese_test.AddTopping("cheese");
    cheese_test.AddTopping("cheese");
    int cheese_count = 0;
    for (std::string topping : cheese_test.GetToppings()) {
      if (topping == "cheese") {
        cheese_count++;
      }
    }
    REQUIRE(cheese_count == 1);
  }
}

TEST_CASE("Sandwich::RemoveTopping test", "[RemoveTopping]") {
  // your tests for Sandwich::RemoveTopping here
  SECTION("Toppings can only be removed before a sauce is added") {
    Sandwich plain_bread;
    for (std::string topping : plain_bread.toppings) {
      Sandwich can_remove_without_sauce;
      can_remove_without_sauce.AddTopping(topping);
      REQUIRE(can_remove_without_sauce.RemoveTopping(topping));
    }
    for (std::string dressing : plain_bread.dressings) {
      Sandwich cannot_remove_with_sauce;
      cannot_remove_with_sauce.AddTopping("lettuce");
      cannot_remove_with_sauce.AddDressing(dressing);
      REQUIRE_FALSE(cannot_remove_with_sauce.RemoveTopping("lettuce"));
    }
  }
  SECTION("A topping can only be removed if it actually exists") {
    Sandwich plain_bread;
    for (std::string topping : plain_bread.toppings) {
      Sandwich cannot_remove_nonexistent_topping;
      REQUIRE_FALSE(cannot_remove_nonexistent_topping.RemoveTopping(topping));
      cannot_remove_nonexistent_topping.AddTopping(topping);
      REQUIRE(cannot_remove_nonexistent_topping.RemoveTopping(topping));
      REQUIRE_FALSE(cannot_remove_nonexistent_topping.RemoveTopping(topping));
    }
  }
}

TEST_CASE("Sandwich::AddDressing test", "[AddDressing]") {
  // your tests for Sandwich::AddDressing here
  SECTION("There shouldn't be duplicate dressings") {
    Sandwich plain_bread;
    for (std::string dressing : plain_bread.dressings) {
      Sandwich duplicate_dressing_test;
      duplicate_dressing_test.AddTopping("cheese");
      duplicate_dressing_test.AddDressing("mustard");
      duplicate_dressing_test.AddDressing(dressing);
      duplicate_dressing_test.AddDressing(dressing);
      duplicate_dressing_test.AddDressing("vinegar");
      duplicate_dressing_test.AddTopping("tomato");
      duplicate_dressing_test.AddDressing(dressing);
      int dressing_count = 0;
      for (std::string sauce : duplicate_dressing_test.GetDressings()) {
        if (sauce == dressing) {
          dressing_count++;
        }
      }
      REQUIRE(dressing_count == 1);
    }
  }
  SECTION("A dressing shouldn't be added until there's at least one topping") {
    Sandwich topping_before_dressing_test;
    for (std::string dressing : topping_before_dressing_test.dressings) {
      REQUIRE_FALSE(topping_before_dressing_test.AddDressing(dressing));
    }
  }
}
