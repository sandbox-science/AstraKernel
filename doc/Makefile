SOURCES=main.tex
OUTPUT=AstraKernelManual.pdf
INTERMEDIATE_DIR=bin

all: build

# How to generate the PDF from the LaTeX source
build:
	mkdir -p $(INTERMEDIATE_DIR)
	pdflatex -jobname=AstraKernelManual -output-directory=$(INTERMEDIATE_DIR) $(SOURCES)
	mv $(INTERMEDIATE_DIR)/$(OUTPUT) .

# Clean up intermediate files
clean:
	rm -f $(INTERMEDIATE_DIR)/*