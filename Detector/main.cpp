/*
 * mian.cpp
 *
 *  Created on: 2016. 7. 18.
 *      Author: cmk
 */

#include "main.h"


int main(int argc, char** argv){

	///////주변장치 연결확인

	printf("\n주변장치와 연결여부를 확인합니다...");

	while(0){

		int conn_state=0;
		char   ch_key=0;

		if( (conn_state = CHECK_DEVICES()) != FULL_CON){

			if(conn_state & 0001){
				fprintf(stderr,"\n캠이 연결되어 있지 않습니다. 캠 사용여부를 다시 확인 후 y를 입력하세요."
						"(\n프로그램 종료를 원하면 n을 입력하세요.");

			}
			else if(conn_state == ( CON_CAM | CON_LED) ){
				fprintf(stderr,"\n스피커가 연결되지 않았습니다.");

			}
			else if(conn_state == (CON_CAM|CON_SPK)){
				fprintf(stderr,"\nLED가 연결되지 않았습니다.");

			}


			while(1){
				ch_key = getchar();

				if(ch_key == 'y'){
					break;
				}

				else if(ch_key == 'n'){
					printf("프로그램을 종료합니다.");
					return 0;
				}
				else
					fprintf(stderr,"\n y나 n만 입력바랍니다.");

			}

			for(int i=0 ; i < 5 ; i++){
				//sleep(1000);
				printf("\n재 연결 시도중입니다..............\n");
			}

		}
		else{
			printf("모든 장치가 연결되어 있습니다.");
			break;
		}
	}

	/////주변 라즈베리 탐색

	server();
	client();


	while(1){

	}




}
