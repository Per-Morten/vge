#include <catch.h>
#include <vge_slot_map.h>

TEST_CASE("Size is empty upon creation", "[slot_map]")
{
    VGE::SlotMap<int> map;
    REQUIRE(map.Size() == 0);
    REQUIRE(map.Capacity() == 2);
}

TEST_CASE("Size increments upon insert", "[slot_map]")
{
    VGE::SlotMap<int> map;
    auto idx = map.Insert(1);
    REQUIRE(map.Size() == 1);
}

TEST_CASE("Size decrements upon deletion", "[slot_map]")
{
    VGE::SlotMap<int> map;
    auto idx = map.Insert(1);
    map.Remove(idx);
    REQUIRE(map.Size() == 0);
}

TEST_CASE("Assert on double remove of ID", "[slot_map]")
{
    VGE::SlotMap<int> map;
    auto idx = map.Insert(1);
    map.Remove(idx);
    REQUIRE_THROWS(map.Remove(idx));
}

TEST_CASE("Nullptr on deleted accessing ID", "[slot_map]")
{
    VGE::SlotMap<int> map;
    auto idx = map.Insert(1);
    map.Remove(idx);
    REQUIRE(map[idx] == nullptr);
}

TEST_CASE("Ensure contiguous memory", "[slot_map]")
{
    // TODO: Expand this case, to ensure that it still works correctly after
    //       relocating, etc.
    VGE::SlotMap<int> map;
    auto idx = map.Insert(0);
    auto idx2 = map.Insert(1);
    REQUIRE(map[0] == 0);
    REQUIRE(map[1] == 1);
    map.Remove(idx);
    REQUIRE(map[0] == 1);
}
