#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:Seed\.spawn expected 1 argument\nFailed to execute child process "asihfieuhgieuhgw" \(No such file or directory\)\nSeed\.include expected 1 argument, got 0\nFile not found: 1\nFile not found: \nSeed\.include expected 1 argument, got 2\nSeed\.include expected 1 argument, got 2\nprint expected 1 argument, got 0\nprint expected 1 argument, got 2\nprint expected 1 argument, got 2\nprint expected 1 argument, got 3\nSeed\.introspect expected 1 argument, got 0\nSeed\.introspect expected 1 argument, got 2\nSeed\.check_syntax expected 1 argument, got 0\nSeed\.check_syntax expected 1 argument, got 2
// STDERR:

try{
Seed.spawn();
}catch(e){print(e.message);}
try{
Seed.spawn("asihfieuhgieuhgw");
}catch(e){print(e.message);}

try{
Seed.include();
}catch(e){print(e.message);}
try{
Seed.include(1);
}catch(e){print(e.message);}
try{
Seed.include("");
}catch(e){print(e.message);}
try{
Seed.include(1,2);
}catch(e){print(e.message);}
try{
Seed.include("fail.js","another.js");
}catch(e){print(e.message);}

try{
print();
}catch(e){print(e.message);}
try{
print(1, 2);
}catch(e){print(e.message);}
try{
print("asdf", 2);
}catch(e){print(e.message);}
try{
print(1, 2, 3);
}catch(e){print(e.message);}

try{
Seed.introspect();
}catch(e){print(e.message);}
try{
Seed.introspect(Seed, 5.23);
}catch(e){print(e.message);}

try{
Seed.check_syntax();
}catch(e){print(e.message);}
try{
Seed.check_syntax("5+5", "asdf");
}catch(e){print(e.message);}

try{
Seed.quit(1,2);
}catch(e){print(e.message);}

Seed.quit();
