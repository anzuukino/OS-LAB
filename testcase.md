# multi-args commands
```
head components/history.c components/piping.c components/redirect.c
tar zcvf test.tar.gz components/history.c components/redirect.c components/execute.c 
//error handling
eco 1 2 3
```
# multiple execution 
```
ls -la ; sleep 1 ; echo Hello
```
# redirect to a certain file 
```
sort < components/history.c 
base64 osh > test.txt 
sort < components/history.c > test.txt 
```

# multiple execution with redirect 
```
echo 1111 > 1.txt ; echo 2 > 2.txt ; base64 < osh 
```

# History 
```
ls
!!
!-3 
!4
echo !! 
echo !9
!! ; !! 
// error handlihg 
!!100000
```
# Piping
```
echo 123 | base64 | base64 | base64 
ls -la | less
cat main.c | grep main.c
cat execute.c | grep ls > test.txt 
base64 < main.c | base64 | base64 -d | base64 -d > main2.c
```
