# questions

* are there any gotchas for using std::string\_view instead of util::string\_view
* there are implementations for starts\_with and ends\_with, but their usage appears
exceptionally limited
* is init\_special\_effect actually necessary?
* what even is player\_t::init_scaling()
* what is the intent behind shared? its not consistent
* merge proc and rppm?
* why do cooldowns get initialized in the monk_t constructor, while spells get
initialized at a later point?

# possibilities
* keep all cache invalidation in monk\_t::invalidate\_cache
* automate generation of abilities, passives, talents, from some datasource
to guarantee format

# general shape

* no more header, it feels like a lot of extra work
* public at top, private at bottom
* make sure it's clear what it's in
* dont' repeat the keyword
* namespace monk 1 contains all of the wow-machinery (player, spell, etc defs)
* namespace monk 2 contains all of the simc-machinery (report, module, etc)

# style

* opt to use T* name
* opt to use full names, no abbreviation (unless the name has pretty much been abstracted away ala "os")
* remove all using (it wasn't improving readability) -> maybe change "Base" name
* avoid references in favor of pointers when reasonable for consistency, as much
of implementation already uses them
* limit use of auto, if anything as a test of your own brain :P
* use up all cases (even if not feasible) OR use default. dont do both.
* if you're doing float arithmetic, don't use anything the compiler may think is
an int and then cause int divison in some future timeline. 1.0 is ur bff
* don't pollute the monk_t (or any other generic) namespaces. put the code in
the ability if at all possible (active action, etc)

# labels
* TODO: VERIFY IG -> verify behaviour, possibly changed from current impl
* TODO: VERIFY SC -> verify the logic in simc, might have made error
* TODO: INACTIVE -> currently disabled due to potentially being irrelevant
* TODO: STUB -> stubbed for compilation, logic or data needed

####
CURRENT MARK: monk\_t::init\_base\_stats()
