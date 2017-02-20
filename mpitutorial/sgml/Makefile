all: clean html pdf

html:	code.sgml html1


html0: tutorial.sgml
	-rm -fr html html.junk
	openjade -t html -d "/usr/share/sgml/docbook/utils-0.6.14/docbook-utils.dsl#html" -wall ../tutorial.sgml
	(cd html && ln -s ../graphics/* .)


# das geht unter suse 9.3
html1: tutorial.sgml
	-rm -fr html 
	-mkdir html
	(cd html && ln -s ../*.sgml .)
	(cd html && docbook2html -p /usr/bin/openjade tutorial.sgml)

#	(cd html && ln -s ../graphics/* .)


html2: tutorial.sgml
	-rm -fr html 
	-mkdir html
	(cd html && ln -s ../graphics/* .)
	ln -s ../tutorial.sgml html/tutorial.sgml
	(cd html && db2html -h tutorial.sgml)

pdf:	code.sgml pdf1
	cp pdf/tutorial.pdf ..

# das geht unter suse 9.3
pdf0: tutorial.sgml
	-rm -fr pdf
	-mkdir pdf
	(cd pdf && ln -s ../graphics/* .)
	(cd pdf && ln -s ../*.sgml .)
	(cd pdf && openjade -t tex -d "/usr/share/sgml/docbook/utils-0.6.14/docbook-utils.dsl" -wall tutorial.sgml && pdfjadetex tutorial.tex && pdfjadetex tutorial.tex && pdfjadetex tutorial.tex && pdfjadetex tutorial.tex && pdfjadetex tutorial.tex)

pdf1: tutorial.sgml
	-rm -fr pdf 
	-mkdir pdf
	(cd pdf && ln -s ../graphics/* .)
	(cd pdf && ln -s ../*.sgml .)
	(cd pdf && docbook2pdf  -p /usr/bin/openjade tutorial.sgml)

pdf2: tutorial.sgml
	-rm -fr pdf 
	-mkdir pdf
	(cd pdf && ln -s ../graphics/* .)
	ln -s ../*.sgml pdf
	(cd pdf && db2pdf --pdf -x -V tutorial.sgml)

pdf3: tutorial.sgml
	-rm -fr pdf 
	-mkdir pdf
	(cd pdf && ln -s ../graphics/* .)
	ln -s ../*.sgml pdf
	(cd pdf && docbook2pdf tutorial.sgml)

code.sgml: code2sgml.sh ../*.c ../*.h
	./code2sgml.sh


xpdf:
	xpdf -paper A4 pdf/tutorial.pdf &

rtf: tutorial.sgml 
	-mkdir rtf
	#db2rtf  -o rtf  -d "docbook-utils.dsl#print" tutorial.sgml
	#(cd rtf ; docbook2rtf ../tutorial.sgml)
	#(cd rtf; openjade -d /usr/share/sgml/docbook/utils-0.6.14/docbook-utils.dsl -t rtf -o tutorial.rtf -G -w all ../tutorial.sgml)
	(cd rtf; openjade -d /usr/share/sgml/docbook/dsssl-stylesheets-1.79/print/docbook.dsl -t rtf -o tutorial.rtf -G -w all ../tutorial.sgml)



clean:
	rm -fr html rtf pdf tutorial tutorial.junk *~ code.sgml

