#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:1\n-124\n129\n-1235\n1387\n-123435\n133487\n-16323375\n13873737\n-16325\n33737\n-16325\n33737\n12\n143\n-1853\.12329101562\n-134543853\.12334\n8\n\["åäö","\/etc\/fstab"\]\n\["1","2","3"\]\n\["1","2","3"\]\n\["1","2","3"\]\n\["1","2","3"\]\nthanks,for,all,the,fish\n5\n0\n123\n0\n1\n42\n1\n4\n15\n120\n120\.2357\n42\n19
// STDERR:

Everything = imports.gi.Everything;
GObject = imports.gi.GObject;

JSON = imports.JSON;

with(Everything)
{
print(test_boolean(true));
print(test_int8(-124));
print(test_uint8(129));
print(test_int16(-1235));
print(test_uint16(1387));
print(test_int32(-123435));
print(test_uint32(133487));
print(test_int64(-16323375));
print(test_uint64(13873737));
print(test_int(-16325));
print(test_uint(33737));
print(test_long(-16325));
print(test_ulong(33737));
print(test_ssize(12));
print(test_size(143));
print(test_float(-1853.12334));
print(test_double(-134543853.12334));
//Need support for: print(test_timet(1853));
print(test_gtype(8));

print(JSON.stringify(test_filename_return()));
print(JSON.stringify(test_glist_nothing_return()));
print(JSON.stringify(test_glist_nothing_return2()));
print(JSON.stringify(test_glist_container_return()));
print(JSON.stringify(test_glist_everything_return()));

//test_glist_nothing_in([2, 3, 4]);

print(test_strv_out());

// These are broken... Robb's working on them:
print(test_closure(function () { return [GObject.TYPE_INT, 5]; }));
print(test_closure_one_arg(function (a) { return [GObject.TYPE_INT, a]; }));

print(test_value_return(123).get_int());

print(TestEnum.VALUE1);
print(TestEnum.VALUE2);
print(TestEnum.VALUE3);
print(TestFlags.FLAG1);
print(TestFlags.FLAG2);
// Err.... wot? The gir has 2 in twice... print(TestFlags.Flag3);

TestStructA.some_int = 15;
print(TestStructA.some_int);
TestStructA.some_int8 = 120;
print(TestStructA.some_int8);
TestStructA.some_double = 120.2357;
print(TestStructA.some_double);
TestStructA.some_enum = TestEnum.VALUE3;
print(TestStructA.some_enum);

TestStructB.some_int8 = 19;
print(TestStructB.some_int8);
//TestStructB.nested_a.some_double = 134.3455;
//print(TestStructB.nested_a.some_double);
}
