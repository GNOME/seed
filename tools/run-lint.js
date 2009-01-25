#!/usr/bin/env seed

Seed.import_namespace("Gio");

Seed.include("jslint.js");

var read_file = Gio.simple_read("run-lint.js");
read_file = read_file.replace("#!/usr/bin/env seed","");
Seed.print(read_file);

if(JSLINT(read_file, {white:false, passfail: false}) == 0)
{
	for(no in JSLINT.errors)
	{
		if(!JSLINT.errors[no])
			continue;
			
		Seed.print(JSLINT.errors[no].reason);
	}
}
