@echo off
echo ################################
echo find "123" 123.txt
find "123" 123.txt

if errorlevel 1 (
	echo NOT FOUND
) else (
	echo SUCCESS
)

echo ################################
echo find "0123" 123.txt
find "0123" 123.txt

if errorlevel 1 (
	echo NOT FOUND
) else (
	echo SUCCESS
)
echo ################################
echo ### find "123" 123.txt /V
echo 
find "123" 123.txt /V

if errorlevel 1 (
	echo NOT FOUND
) else (
	echo SUCCESS
)

echo ################################
echo ### find "0123" 123.txt /V 
find "0123" 123.txt /V 

if errorlevel 1 (
	echo NOT FOUND
) else (
	echo SUCCESS
)

echo ################################
echo ### find "123" 123.txt /y 
find "123" 123.txt /y 

if errorlevel 2 (
	echo NOT FOUND
) else (
	echo SUCCESS
)

echo ################################
echo ### find  "123" NOEXISTINMG123.txt
find  "123" NOEXISTINMG123.txt

if errorlevel 2 (
	echo NOT FOUND
) else (
	echo SUCCESS
)

echo ################################
echo ### find  1 22 333
find  1 22 333

if errorlevel 3 (
	echo NOT FOUND
) else (
	echo SUCCESS
)

echo ################################
echo ### find /?
find  /?

if errorlevel 1 (
	echo NOT FOUND
) else (
	echo SUCCESS
)
