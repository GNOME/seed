#!/usr/bin/env seed

Gio = imports.gi.Gio;

Seed.include("jslint.js");

function runLint(filename)
{
	var read_file = Gio.simple_read(filename);

	read_file = read_file.replace("#!/usr/bin/env seed","");
	
	if(JSLINT(read_file, {white:false, passfail: false, eqeqeq: false, forin: false}))
		return;
	
	print("################################################################################");
	print("            " + filename);
	
	for(no in JSLINT.errors)
	{
		var err = JSLINT.errors[no];
	
		if(!err || !err.reason)
			continue;
	
		printf("%d:%d\t%s",err.line,err.character,err.reason);
	}
	
	return JSLINT.errors.length;
}

for(a in Seed.argv)
{
	if(a >= 2)
		runLint(Seed.argv[a]);
}
