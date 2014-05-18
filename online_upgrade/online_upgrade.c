#include "online_upgrade.h"
#include <stdio.h>
#include <curl/curl.h>
#include <string.h>


size_t my_write_func(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	return fwrite(ptr, size, nmemb, stream);
}

size_t my_read_func(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	return fread(ptr, size, nmemb, stream);
}


int progress_func(char *progress_data,
					double t, /* dltotal */
					double d, /* dlnow */
					double ultotal,
					double ulnow)
{
	printf("%s %.2fK of %.2fK (%.2f %%)\n", progress_data, d/1024, t/1024, d*100.0/t);
	return 0;
}

int main(int argc,char **argv) 
{
	CURL *curl;
	CURLcode result;
	char *progress_data = "-> ";
	curl=curl_easy_init();
	if(!curl) {
		return -1;
	}
	char *url;
	char *auth="bam:bam";
	if(argc > 1) {
		url=argv[1];
	} else {
		url="ftp://192.168.0.110/1.txt";
	}
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_USERPWD, auth);
 
	FILE *fp=fopen("/result","w+");
	if(!fp) {
		perror("fopen");
		return -2;
	}
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15);
	
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_write_func);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, my_read_func);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0/*FALSE*/);
	curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_func);
	curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, progress_data);
	
	result=curl_easy_perform(curl);
	if (CURLE_OK != result) {
	    printf("Error happened, errno is: %d\n", result);
	}
	curl_easy_cleanup(curl);
	fclose(fp);
	
	return 0;
}
