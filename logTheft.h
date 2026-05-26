void logTheft(struct tm *current){
	FILE *fptr = fopen("ruby_theft.log", "a"); //error - need to create a new file, currently outputs time several times

	if(fptr == NULL){
		perror("Failed to open log file");
		return;
	}

	int hours = current->tm_hour;
	int minutes = current->tm_min;
	int seconds = current->tm_sec;
	int wday = current->tm_wday;
	int mday = current->tm_mday;
	int month = current->tm_mon + 1;
	int year = current->tm_year + 1900;

	char *dayName = days[current->tm_wday];
	char *monthName = months[current->tm_mon];


	fprintf(fptr, "Date and time of attempted theft: \n");
	fprintf(fptr, "%s %d %s %d\n", dayName, mday, monthName, year);
	fprintf(fptr, "%02d hours - %02d minutes - %02d seconds\n\n", hours, minutes, seconds);
	
	fclose(fptr);

}