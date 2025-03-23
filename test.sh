#!/bin/bash
export LD_LIBRARY_PATH=/opt/glibc-2.40/lib
# Ajouté pour les modules NSS
export GCONV_PATH=/opt/glibc-2.40/lib/gconv
export LOCPATH=/opt/glibc-2.40/lib/locale
"$@"