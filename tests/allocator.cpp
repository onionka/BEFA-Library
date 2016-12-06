////
//// Created by miro on 12/4/16.
////
//
////
//// Created by miro on 12/3/16.
////
//
//#include <iostream>
//#include <gtest/gtest.h>
//#include <memory>
//
//#include <befa/utils/visitor.hpp>
//#include <befa/assembly/asm_arg_parser.hpp>
//#include <befa/utils/factory.hpp>
//#include <befa/assembly/instruction.hpp>
//#include <befa/utils/bucket_allocator.hpp>
//
//namespace {
//
//TEST(AllocatorTests, StackAllocator) {
//  befa::StackAllocator<int, 128> stack_alloc;
//
//  EXPECT_EQ(stack_alloc.size(), 0);
//  auto ptr = static_cast<int *>(stack_alloc.allocate());
//  EXPECT_EQ(stack_alloc.size(), 1);
//  ASSERT_TRUE(ptr);
//  *ptr = 5;
//  ASSERT_EQ(*ptr, 5);
//
//  stack_alloc.destroy(ptr);
//  EXPECT_EQ(stack_alloc.size(), 0);
//}
//
//TEST(AllocatorTests, StackAllocatorMore) {
//  befa::StackAllocator<int, 2> stack_alloc;
//  int *ptr1, *ptr2;
//  {
//    EXPECT_EQ(stack_alloc.size(), 0);
//    ptr1 = static_cast<int *>(stack_alloc.allocate());
//    EXPECT_EQ(stack_alloc.size(), 1);
//
//    ASSERT_TRUE(ptr1);
//    *ptr1 = 5;
//    ASSERT_EQ(*ptr1, 5);
//  }
//
//  {
//    EXPECT_EQ(stack_alloc.size(), 1);
//    ptr2 = static_cast<int *>(stack_alloc.allocate());
//    EXPECT_EQ(stack_alloc.size(), 2);
//
//    ASSERT_TRUE(ptr2);
//    *ptr2 = 5;
//    ASSERT_EQ(*ptr2, 5);
//  }
//
//  stack_alloc.destroy(ptr1);
//  EXPECT_EQ(stack_alloc.size(), 1);
//
//  stack_alloc.destroy(ptr2);
//  EXPECT_EQ(stack_alloc.size(), 0);
//}
//
//TEST(MallocatorTests, MallocTest) {
//  befa::Mallocator<int> mallocator;
//
//  EXPECT_EQ(mallocator.size(), 0);
//  auto ptr = static_cast<int *>(mallocator.allocate());
//  EXPECT_EQ(mallocator.size(), 1);
//
//  ASSERT_TRUE(ptr);
//  *ptr = 5;
//  ASSERT_EQ(*ptr, 5);
//
//  mallocator.destroy(ptr);
//  EXPECT_EQ(mallocator.size(), 0);
//}
//
//TEST(MallocatorTests, MallocTestMore) {
//  befa::Mallocator<int> mallocator;
//  int *ptr1, *ptr2;
//  {
//    EXPECT_EQ(mallocator.size(), 0);
//    ptr1 = static_cast<int *>(mallocator.allocate());
//    EXPECT_EQ(mallocator.size(), 1);
//
//    ASSERT_TRUE(ptr1);
//    *ptr1 = 5;
//    ASSERT_EQ(*ptr1, 5);
//  }
//
//  {
//    EXPECT_EQ(mallocator.size(), 1);
//    ptr2 = static_cast<int *>(mallocator.allocate());
//    EXPECT_EQ(mallocator.size(), 2);
//
//    ASSERT_TRUE(ptr2);
//    *ptr2 = 5;
//    ASSERT_EQ(*ptr2, 5);
//  }
//
//  mallocator.destroy(ptr1);
//  EXPECT_EQ(mallocator.size(), 1);
//
//  mallocator.destroy(ptr2);
//  EXPECT_EQ(mallocator.size(), 0);
//}
////struct AllocatedObject;
////
////namespace details {
////using allocator_type = befa::AltAllocator<
////    befa::StackAllocator<AllocatedObject, 512>,
////    befa::Mallocator<AllocatedObject>
////>;
////
////static allocator_type global_allocator;
////}
////
////struct AllocatedObject {
////  void *operator new(size_t) {
////    printf("new has been called\n");
////    return details::global_allocator.allocate();
////  }
////
////  void operator delete(void *ptr) {
////    details::global_allocator.destroy((AllocatedObject *)ptr);
////    printf("delete has been called\n");
////  }
////
////  int a, b;
////  std::string wtf;
////};
////
////TEST(RealNewTests, NewTest) {
////  auto object = new AllocatedObject;
////
////  ASSERT_NE(object, nullptr);
////  object->a = 5;
////  object->wtf = "wtf";
////
////  auto obj2 = std::shared_ptr<AllocatedObject>(
////      new AllocatedObject, [] (AllocatedObject *ptr) {
////        details::global_allocator.destroy(ptr);
////      }
////  );
////  ASSERT_TRUE((bool) obj2);
////
////  memcpy(obj2.get(), object, sizeof(AllocatedObject));
////
////  ASSERT_EQ(obj2->a, 5);
////  ASSERT_EQ(obj2->wtf, "wtf");
////
////  delete object;
////}
//
//}
