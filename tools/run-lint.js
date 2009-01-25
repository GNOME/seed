#!/usr/bin/env seed

Seed.import_namespace("Gio");

Seed.include("jslint.js");

function runLint(filename)
{
	var read_file = Gio.simple_read(filename);

	read_file = read_file.replace("#!/usr/bin/env seed","");
	
	Seed.print("=====================================");
	Seed.print("    " + filename);
	Seed.print("=====================================");

	if(JSLINT(read_file, {white:false, passfail: false, eqeqeq: false, forin: false}))
		return;
	
	for(no in JSLINT.errors)
	{
		var err = JSLINT.errors[no];
	
		if(!err || !err.reason)
			continue;
	
		Seed.printf("%d:%d\t%s",err.line,err.character,err.reason);
	}
	
	return JSLINT.errors.length;
}

runLint("run-lint.js");
