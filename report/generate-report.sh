#!/bin/bash

./compile-tex.sh _root.tex report.tex   && \
pdflatex report.tex     && \
bibtex report.tex       && \
pdflatex report.tex     && \
pdflatex report..tex
