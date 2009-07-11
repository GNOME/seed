#!/usr/bin/env seed

readline = imports.readline;
mpfr = imports.mpfr;

print("This example is currently incomplete.");
Seed.quit();

var isAtom = function(a)
{
	return typeof a == 'string' || typeof a == 'number' || typeof a == 'boolean';
};

var car = function(s)
{
	return s[0];
};

var cdr = function(s)
{
	return s[1];
};

var firstSubExp = function(exp)
{
	return car(cdr(exp));
};

var secondSubExp = function(exp)
{
	return car(cdr(cdr(exp)));
};

var plus = function(n, m)
{
	return parseInt(n, 10)+parseInt(m, 10);
}

var minus = function(n, m)
{
	return parseInt(n, 10)-parseInt(m, 10);
}

var times = function(n, m)
{
	return parseInt(n, 10) * parseInt(m, 10);
}

var divide = function(n, m)
{
	return parseInt(n, 10) / parseInt(m, 10);
}

var nop = function(n, m)
{
	return 0;
}

var atomToFunction = function(x)
{
	if (x === '+')
		return plus;
	if (x === '-')
		return minus;
	if (x === '*')
		return times;
	if (x === '/')
		return divide;
	else
	{
		print("Syntax Error");
		repl(); // TODO: cheating
	}
}

var repl = function()
{
	while (1)
	{
		a = readline.readline(">> ");
		if (a === "quit")
			return;
		print(value(parse(a)));
	}
}

var value = function (exp)
{
	return isAtom(exp) ? exp :
		atomToFunction(car(exp)) (
			value(firstSubExp(exp)),
			value(secondSubExp(exp)));		 
}

var parse = function (x)
{
	var tx = /\s*(\(|\)|[^\s()]+|$)/g, result;
	tx.lastIndex = 0;
	
	result = function list()
	{
		var head = null,
			neo  = null,
			r    = tx.exec(x),
			sexp = (r && r[1]) || '',
			tail = null;

		if (sexp != '(')
		{
			return sexp;
		}
		while (true)
		{
			sexp = list();
			
			if (sexp === '' || sexp == ')')
			{
				return head;
			}
			
			neo = [sexp];
			
			if (tail)
			{
				tail[1] = neo;
			}
			else
			{
				tail = head = neo;
			}
			
			tail = neo;
		}
	}();
	
	parse.lastIndex = tx.lastIndex;
	return result;
};

var eval = function(exp)
{
	return value(s(exp));
}

repl();
