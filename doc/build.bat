@echo off
mkdir bin
pdflatex -jobname=AstraKernelManual -output-directory=bin main.tex
move bin\AstraKernelManual.pdf AstraKernelManual.pdf
