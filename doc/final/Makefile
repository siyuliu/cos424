MAIN = final

all:
	pdflatex ${MAIN}
	bibtex ${MAIN}
	pdflatex ${MAIN}
	pdflatex ${MAIN}

.PHONY: clean

clean:
	rm *.aux *.bbl *.blg *.log *.pdf