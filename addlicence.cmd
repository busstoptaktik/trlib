mkdir xTR_INC
move TR_INC\*.h xTR_INC
cd xTR_INC
for %%f in (*.h) do copy ..\LICENCE.isc+%%f ..\TR_INC\%%f
cd ..

mkdir xTR_SRC
move TR_SRC\*.c xTR_SRC
cd xTR_SRC
for %%f in (*.c) do copy ..\LICENCE.isc+%%f ..\TR_SRC\%%f
cd ..

mkdir xAPI
move TR_SRC\API\*.? xAPI
cd xAPI
for %%f in (*.?) do copy ..\LICENCE.isc+%%f ..\TR_SRC\API\%%f
cd ..

del xTR_INC\*.h
del xTR_SRC\*.c
del xAPI\*.?

rmdir xTR_INC
rmdir xTR_SRC
rmdir xAPI
