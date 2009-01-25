#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:Seed\.spawn expected 1 argument\nFailed to execute child process "asihfieuhgieuhgw" \(No such file or directory\)\nSeed\.include expected 1 argument, got 0\nFile not found: 1\.000000\nFile not found: \nSeed\.include expected 1 argument, got 2\nSeed\.include expected 1 argument, got 2\nSeed\.print expected 1 argument, got 0\nSeed\.print expected 1 argument, got 2\nSeed\.print expected 1 argument, got 2\nSeed\.print expected 1 argument, got 3\nSeed\.introspect expected 1 argument, got 0\nSeed\.introspect expected 1 argument, got 2\nSeed\.check_syntax expected 1 argument, got 0\nSeed\.check_syntax expected 1 argument, got 2
// STDERR:
// Returns: 0
// STDIN:
// STDOUT:Seed\.include expected 1 argument, got 0\nFile not found: 1\.000000\.\n\nFile not found: \.\n\nSeed\.include expected 1 argument, got 2\nSeed\.include expected 1 argument, got 2\nSeed\.print expected 1 argument, got 0\nSeed\.print expected 1 argument, got 2\nSeed\.print expected 1 argument, got 2\nSeed\.print expected 1 argument, got 3\nSeed\.introspect expected 1 argument, got 0\nSeed\.introspect expected 1 argument, got 2\nSeed\.check_syntax expected 1 argument, got 0\nSeed\.check_syntax expected 1 argument, got 2
// STDERR:

try{
Seed.spawn();
}catch(e){Seed.print(e.message);}
try{
Seed.spawn("asihfieuhgieuhgw");
}catch(e){Seed.print(e.message);}

try{
Seed.include();
}catch(e){Seed.print(e.message);}
try{
Seed.include(1);
}catch(e){Seed.print(e.message);}
try{
Seed.include("");
}catch(e){Seed.print(e.message);}
try{
Seed.include(1,2);
}catch(e){Seed.print(e.message);}
try{
Seed.include("fail.js","another.js");
}catch(e){Seed.print(e.message);}

try{
Seed.print();
}catch(e){Seed.print(e.message);}
try{
Seed.print(1, 2);
}catch(e){Seed.print(e.message);}
try{
Seed.print("asdf", 2);
}catch(e){Seed.print(e.message);}
try{
Seed.print(1, 2, 3);
}catch(e){Seed.print(e.message);}

try{
Seed.introspect();
}catch(e){Seed.print(e.message);}
try{
Seed.introspect(Seed, 5.23);
}catch(e){Seed.print(e.message);}

try{
Seed.check_syntax();
}catch(e){Seed.print(e.message);}
try{
Seed.check_syntax("5+5", "asdf");
}catch(e){Seed.print(e.message);}

try{
Seed.quit(1,2);
}catch(e){Seed.print(e.message);}

Seed.quit();
