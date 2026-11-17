## 자동화 무인 우산 보관함 프로젝트 - 슈룹터
---

### 프로젝트 개요
본 프로젝트는 사용자가 우산을 안전하게 보관하고 필요 시 쉽게 찾을 수 있도록 하는 무인 우산 보관함 시스템 개발을 목표로 설계하였다.
슬롯 선택 후 비밀번호를 등록하면 우산을 보관할 수 있으며, 인증 절차를 통해 본인의 우산을 안전하게 회수할 수 있도록 한다.

---

## Tech Stack

![C++](https://img.shields.io/badge/C++-00599C?style=flat&logo=cplusplus&logoColor=white)
![Visual Studio](https://img.shields.io/badge/Visual%20Studio-5C2D91?style=flat&logo=visualstudio&logoColor=white)
![Arduino](https://img.shields.io/badge/Arduino%20Mega2560-00878F?style=flat&logo=arduino&logoColor=white)
![TIA Portal](https://img.shields.io/badge/TIA%20Portal-009999?style=flat&logo=siemens&logoColor=white)

### Software
| 구분 | 사용 툴 / 언어 | 용도 |
|---|---|---|
| PC 프로그램 | Visual Studio / MFC (C++) | 전체 시스템 제어 및 모니터링 GUI |
| PLC | Siemens TIA Portal | 센서·공정 시퀀스 제어 |
| HMI | TIA Portal (WinCC Basic) | 터치 패널 화면 설계 |
| 펌웨어 | Arduino IDE (C++) | 포토 인터럽터 센서 인식, 모터 정/역회전, 도어락 제어 |

### Hardware
| 장비 | 모델 | 용도 |
|---|---|---|
| PLC | SIMATIC S7-1200 | 공정 제어 |
| HMI | SIMATIC KTP700 Basic | 터치 패널 UI |
| MCU | Arduino Mega2560 | 서브 컨트롤러 |
| 센서 | 포토 인터럽터 + 슬릿 디스크 (자체 제작) | 회전 위치 검출 및 슬롯 인덱싱 |
| 모터 | (KWC-0025 BLDC / L298N) | 정/역회전 구동 |
| 도어락 | JF-0530B-24V + 릴레이 모듈 | 잠금 제어 |

---

프로젝트 기획 배경, 시스템 구성, 모델 학습 과정 및 구현 상세 내용은 아래 블로그에 정리되어 있습니다.
[프로젝트 상세 설명 보러 가기] (https://xiin.tistory.com/47)
