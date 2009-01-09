#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:1\.000000\n-124\.000000\n129\.000000\n-1235\.000000\n1387\.000000\n-123435\.000000\n133487\.000000\n-16323375\.000000\n13873737\.000000\n-16325\.000000\n33737\.000000\n-16325\.000000\n33737\.000000\n12\.000000\n143\.000000\n-1853\.123291\n-134543853\.123340\n8\.000000\n\["åäö","\/etc\/fstab"\]\n\["1","2","3"\]\n\["1","2","3"\]\n\["1","2","3"\]\n\["1","2","3"\]\n1,2,3\n5\.000000\n0\.000000\n123\.000000\n0\.000000\n1\.000000\n42\.000000\n1\.000000\n4\.000000\n15\.000000\n120\.000000\n120\.235700\n42\.000000\n19\.000000
// STDERR:

Seed.import_namespace("Everything");

with(Everything)
{
Seed.print(test_boolean(true));
Seed.print(test_int8(-124));
Seed.print(test_uint8(129));
Seed.print(test_int16(-1235));
Seed.print(test_uint16(1387));
Seed.print(test_int32(-123435));
Seed.print(test_uint32(133487));
Seed.print(test_int64(-16323375));
Seed.print(test_uint64(13873737));
Seed.print(test_int(-16325));
Seed.print(test_uint(33737));
Seed.print(test_long(-16325));
Seed.print(test_ulong(33737));
Seed.print(test_ssize(12));
Seed.print(test_size(143));
Seed.print(test_float(-1853.12334));
Seed.print(test_double(-134543853.12334));
//Need support for: Seed.print(test_timet(1853));
Seed.print(test_gtype(8));

Seed.print(JSON.stringify(test_filename_return()));
Seed.print(JSON.stringify(test_glist_nothing_return()));
Seed.print(JSON.stringify(test_glist_nothing_return2()));
Seed.print(JSON.stringify(test_glist_container_return()));
Seed.print(JSON.stringify(test_glist_everything_return()));

//test_glist_nothing_in([2, 3, 4]);

Seed.print(test_strv_out());

// These are broken... Robb's working on them:
Seed.print(test_closure(function () { return [GObject.TYPE_INT, 5]; }));
Seed.print(test_closure_one_arg(function (a) { return [GObject.TYPE_INT, a]; }));

Seed.print(test_value_return(123).get_int());

Seed.print(TestEnum.Value1);
Seed.print(TestEnum.Value2);
Seed.print(TestEnum.Value3);
Seed.print(TestFlags.Flag1);
Seed.print(TestFlags.Flag2);
// Err.... wot? The gir has 2 in twice... Seed.print(TestFlags.Flag3);

TestStructA.some_int = 15;
Seed.print(TestStructA.some_int);
TestStructA.some_int8 = 120;
Seed.print(TestStructA.some_int8);
TestStructA.some_double = 120.2357;
Seed.print(TestStructA.some_double);
TestStructA.some_enum = TestEnum.Value3;
Seed.print(TestStructA.some_enum);

TestStructB.some_int8 = 19;
Seed.print(TestStructB.some_int8);
//TestStructB.nested_a.some_double = 134.3455;
//Seed.print(TestStructB.nested_a.some_double);
}
