//
// Created by miro on 16/10/16.
//


#include <gtest/gtest.h>
#include <befa/utils/observer.hpp>

#include "fixtures.hpp"

namespace {


TEST(ObservableTest, RXTest) {
    RxSubject<int> subj;
    RxObservable<int> o$(subj);

    o$.subscribe([](const int &val) {
      ASSERT_EQ(10, val);
    });

    subj.update(10);
}

TEST(ObservableTest, SubjectAsObservable) {
    RxSubject<int> subj;
    RxObservable<int> o$ = subj.toObservable();

    o$.subscribe([](const int &val) {
      ASSERT_EQ(10, val);
    });

    subj.update(10);
}

TEST(ObservableTest, SubscriptionTest) {
  typedef RxSubject<std::string> rx_subject_t;
  typedef rx_subject_t::subscription_type subscription_t;
  typedef rx_subject_t::observable_type observable_t;

  rx_subject_t subj;
  observable_t o$(subj);

  std::vector<subscription_t> subscriptions;

  for (int i = 0; i < 200; ++i) {
    subscriptions.push_back(o$.subscribe([] (const std::string &val) {
      ASSERT_EQ("String", val);
    }));
  }

  subj.update("String");

  for (auto &s : subscriptions)
    s.unsubscribe();

  subj.update("WrongString");
}

TEST(ObservableTest, MoveTest) {
  typedef RxSubject<std::string> rx_subject_t;
  typedef rx_subject_t::subscription_type subscription_t;
  typedef rx_subject_t::observable_type observable_t;

  auto create_subject = [] () {
    rx_subject_t subj;
    subj.update("Trololo");
    return subj;
  };

  rx_subject_t subj(create_subject());
  observable_t o$(subj);

  std::vector<subscription_t> subscriptions;
  int count = 0;
  for (int i = 0; i < 200; ++i) {
    subscriptions.push_back(o$.subscribe([&] (const std::string &val) {
      ASSERT_EQ("String", val);
      ++count;
    }));
  }

  subj.update("String");

  ASSERT_EQ(count, 200);

  for (auto &s : subscriptions)
    s.unsubscribe();

  subj.update("WrongString");
}

}  // namespace

