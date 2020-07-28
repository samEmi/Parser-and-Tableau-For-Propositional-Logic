# Parser-and-Tableau-For-Propositional-Logic

Input/Tableau.c parses all of propositional logic, outputing whether a formula is part of the language according to the grammar:

prop ::= p | q | r 

BC   ::= v | ^ | >

formula ::= prop | -formula | formula BC formula

where BC stands for binary connecter (or, and, implies)


Furthermore, if a formula in Input.txt is a formula, it outputs whether it is satisfiable using the well-known tableau method.
For example, p ^ q is satisfiable since it's true when both p and q are set to true, whereas -p ^ p is not satisfiable as it is always false regardless
of the values assigned to p.