James Roach 811991452
4730 Project1

This program was implemented in a multiprocess manner using the fork() system call. The interprocess communication was implemented using
message queues. The main process creates a new message queue and recieves a unique msqid. The main process then  enters a for loop 
bounded by the desired number of processes each time calculating how many files each child should read and which ones to read specifically.
After spawning each child uses the word_count() method on all of its assigned files and then uses the msqid from the main process to send a 
message to the message queue containing the sum of the child process's counts. The parent meanwhile reaps all the children using the 
waitpid() system call and then proceeds to read a message off of the message queue for each child. 
