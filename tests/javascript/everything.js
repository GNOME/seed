#!../../src/seed

testsuite = imports.testsuite
Everything = imports.gi.Everything
GObject = imports.gi.GObject
JSON = imports.JSON

assert = testsuite.assert

with(Everything)
{
assert(test_boolean(true) == true)
assert(test_int8(-124) == -124)
assert(test_uint8(129) == 129)
assert(test_int16(-1235) == -1235)
assert(test_uint16(1387) == 1387)
assert(test_int32(-123435) == -123435)
assert(test_uint32(133487) == 133487)
assert(test_int64(-16323375) == -16323375)
assert(test_uint64(13873737) == 13873737)
assert(test_int(-16325) == -16325)
assert(test_uint(33737) == 33737)
assert(test_long(-16325) == -16325)
assert(test_ulong(33737) == 33737)
assert(test_ssize(12) == 12)
assert(test_size(143) == 143)
assert(test_float(-1853.12334) > (-1853.12334 - 0.001))
assert(test_float(-1853.12334) < (-1853.12334 + 0.001))
assert(test_double(-134543853.12334) > (-134543853.12334 - 0.001))
assert(test_double(-134543853.12334) < (-134543853.12334 + 0.001))
//assert(test_timet(1853) == 1853)
assert(test_gtype(8) == 8)

assert(test_filename_return()[0] == "åäö")
assert(test_filename_return()[1] == "/etc/fstab")
assert(test_glist_nothing_return()[0] == "1")
assert(test_glist_nothing_return2()[1] == "2")
assert(test_glist_container_return()[2] == "3")
assert(test_glist_everything_return()[1] == "2")

//test_glist_nothing_in([2, 3, 4])

assert(test_strv_out().toString() == "thanks,for,all,the,fish")

assert(test_closure(function () { return [GObject.TYPE_INT, 5] }) == 5)
assert(test_closure_one_arg(function (a) { return [GObject.TYPE_INT, a] }) == 0)

assert(test_value_return(123).get_int() == 123)

assert(TestEnum.VALUE1 == 0)
assert(TestEnum.VALUE2 == 1)
assert(TestEnum.VALUE3 == 42)
assert(TestFlags.FLAG1 == 1)
assert(TestFlags.FLAG2 == 2)
assert(TestFlags.FLAG3 == 4)

TestStructA.some_int = 15
assert(TestStructA.some_int == 15)
TestStructA.some_int8 = 120
assert(TestStructA.some_int8 == 120)
TestStructA.some_double = 120.2357
assert(TestStructA.some_double > (120.2357 - 0.001))
assert(TestStructA.some_double < (120.2357 + 0.001))
TestStructA.some_enum = TestEnum.VALUE3
assert(TestStructA.some_enum == 42)

TestStructB.some_int8 = 19
assert(TestStructB.some_int8 == 19)
//TestStructB.nested_a.some_double = 134.3455
//print(TestStructB.nested_a.some_double)
}
