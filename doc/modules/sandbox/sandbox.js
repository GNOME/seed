funcs = [{id: "sandbox-context",
	  title: "new sandbox.Context()",
	  description: "Creates a new sandbox context object, which wraps a Seed JavaScript context with it's own global object. By default this global object contains only default JSCore globals (Array, Object, etc...) and has no ability to interact with the outside system. Note the context must be manually destroyed with the <parameter>destroy</parameter> method."},
	 {id: "sandbox-eval",
	  title: "context.eval(source)",
	  description: "Evaluates a string <parameter>source</parameter> with context, returns the result.",
	  params: [
	      {name: "source", description: "The code to evaluate"}
	  ]},
	 {id: "sandbox-addglobals",
	  title: "context.add_globals()",
	  description: "Adds the default Seed globals to the context, including the 'Seed' global object, and the imports object."},
	 {id: "sandbox-destroy",
	  title: "context.destroy()",
	  description: "Destroys the internal context object, and any further usage of the wrapper is an exception"},
	 {id: "sandbox-global",
	  title: "context.global",
	  description: "A project, representing the literal global object of the context, may be freely read from and assigned to"}]
	 
