funcs = [{id: "sqlite-database",
	  title: "new sqlite.Database(filename)",
	  description: "Constructs a new sqlite.Database",
	  params: [
	      {name: "filename", description: "The SQLite database to be opened (or constructed if it does not exist"}
	  ],
	  returns: "A new sqlite.Database object, the <parameter>status</parameter> property will be one of the SQLite status enums"
	 },
	 {id: "sqlite-exec",
	  title:"database.exec(command, callback)",
	  description: "<para>Executes the SQLite <parameter>command</parameter> on the given database. If <parameter>callback</parameter> is defined, it is called with each table entry from the given command, with a single argument. The argument has properties for each value in the returned table entry.</para><para>Keep in mind that, just like in C, it is necessary to sanitize user input in your SQL before passing it to the database.</para>",
	  params: [
	      {name: "command", description: "The SQLite command to evaluate"},
	      {name: "callback", description: "The callback to invoke, should expect one argument and return nothing. <parameter>optional</parameter>"}
	  ],
	  returns: "An SQLite status enum representing the result of the operation"},
	 {id: "sqlite-close",
	  title: "database.close()",
	  description: "Closes an SQLite database and syncs"}];


