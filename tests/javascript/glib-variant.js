//#!/usr/bin/env seed

// test for js object <-> Variant conversion.
// TODO: test edge cases

GLib = imports.gi.GLib;
testsuite = imports.testsuite

// array -> variant conversion tests
var testArray = ["Variantify", 42, [43, "me!"]];

var testArrayVariant = GLib.Variant.new(testArray);

var variantify = testArrayVariant.get_child_value(0).get_variant().get_string();
var fortyTwo = testArrayVariant.get_child_value(1).get_variant().get_int64();
var subArray = testArrayVariant.get_child_value(2).get_variant();
var fortyThree = subArray.get_child_value(0).get_variant().get_int64();
var me = subArray.get_child_value(1).get_variant().get_string();

testsuite.assert(testArrayVariant.is_container());
testsuite.assert(variantify == "Variantify");
testsuite.assert(fortyTwo == 42);
testsuite.assert(fortyThree == 43);
testsuite.assert(me == "me!");

// variant -> array conversion tests
var testArray2 = testArrayVariant.to_js();
testsuite.assert(testArray2[0] == testArray[0]);
testsuite.assert(testArray2[1] == testArray[1]);
testsuite.assert(testArray2[2][0] == testArray[2][0]);
testsuite.assert(testArray2[2][1] == testArray[2][1]);

// object -> variant test
var your = 42;
var testObject = {Let: "me", "be": your, object: 42};

var testObjectVariant = GLib.Variant.new(testObject);
var _let = testObjectVariant.get_child_value(0).get_child_value(0).get_string();
me = testObjectVariant.get_child_value(0).get_child_value(1).get_variant().get_string();
var be = testObjectVariant.get_child_value(1).get_child_value(0).get_string();
var your2 = testObjectVariant.get_child_value(1).get_child_value(1).get_variant().get_int64();
var object = testObjectVariant.get_child_value(2).get_child_value(0).get_string();
fortyTwo = testObjectVariant.get_child_value(2).get_child_value(1).get_variant().get_int64();

testsuite.assert(_let == "Let");
testsuite.assert(me == "me");
testsuite.assert(be == "be");
testsuite.assert(your2 == your);
testsuite.assert(object == "object");
testsuite.assert(fortyTwo == 42);

// variant -> object tests
var testObject2 = testObjectVariant.to_js();
testsuite.assert(testObject2.Let);
testsuite.assert(testObject2.Let == testObject.Let);
testsuite.assert(testObject2.be);
testsuite.assert(testObject2.be == testObject.be);
testsuite.assert(testObject2.object);
testsuite.assert(testObject2.object == testObject.object);

testsuite.checkAsserts(21);





