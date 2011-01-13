<sexpr>:<atom>|<list>;
<list>:(LEFTPAREN)<members>(RIGHTPAREN)|(LEFTPAREN)(RIGHTPAREN);
<members>:<sexpr>|<sexpr><members>;
<atom>:(ID)|(NUM)|(FLOAT)|(STR);