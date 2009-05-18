funcs = [{id: "readline-readline",
	  title: "readline.readline (prompt)",
	  description: "Prompts for one line of input on standard input using <parameter>prompt</parameter> as the prompt.",
	  params: [
	      {name: "prompt", description: "A string to use as the readline prompt"}
	  ],
	  returns: "A string entered on standard input."},
	 {id: "readline-bind",
	  title: "readline.bind (key, function)",
	  description: "Binds <parameter>key</parameter> to <parameter>function</parameter> causing the function to be invokved whenever <parameter>key</parameter> is pressed",
	  params: [
	      {name: "key", description: "A string specifying the key to bind"},
	      {name: "function", description: "The function to invoke when <parameter>key</parameter> is pressed"}
	  ]},
	 {id: "readline-done",
	  title: "readline.done ()",
	  description: "Indicates that readline should finish the current line, and return from <parameter>readline.readline</parameter>. Can be used in callbacks to implement features like multiline editing"},
	 {id: "readline-buffer",
	  title: "readline.buffer()",
	  description: "Retrieve the current readline buffer",
	  returns: "The current readline buffer"},
	 {id: "readline-insert",
	  title: "readline.insert (string)",
	  description: "Inserts <parameter>string</parameter> in to the current readline buffer",
	  params: [
	      {name: "string", description: "The string to insert"}
	  ]}];
