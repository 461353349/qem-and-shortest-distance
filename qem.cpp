// qem.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Matrix.h"
#include "Quadric.h"
#include <string>
#include <iostream>
#include "ShortestEdgeMethod.h"
#define _CRTDBG_MAP_ALLOC

int main(int argc, char* argv[]) {
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

    if(argc != 2) {
        fprintf(stderr,
                "�����������ȷ�����ӣ�exe�� ���㷨��(QEM, Shortest distance)\n");
        return -1;
    }

    if(strcmp(argv[1], "QEM") == 0) {
        Quadrics quadrics;

        Quadrics originQuadrics;
        printf("�������ļ�·��:\n");
        std::string path;
        std::cin>>path;
        if(path.empty() == true) return -1;
        quadrics.readObj(path);
        originQuadrics.readObj(path);
        fprintf(stderr, "����%d���㣬%d��\n", quadrics.getVerticesNum(), quadrics.getFacesNum());
        quadrics.initQMatrices();
        quadrics.selectPairs();
        printf("������Ŀ��ģ�͵Ķ�����\n");

        int targetNums;
        scanf("%d", &targetNums);
        printf("��ʼ��\n");
        quadrics.constructContract(targetNums);
        std::string pathOutput = path.substr(0, path.find_last_of('.')) + "_after.obj";
        quadrics.write_smf(pathOutput.c_str());
        fprintf(stderr, "�򻯳ɹ�\n");

        double e = quadrics.calculateSimilarity(quadrics);
        printf("���ƶ�Ϊ%e\n", e);
    } else if(strcmp(argv[1], "Shortest distance") == 0) {
        ShortestEdgeMethod shortestEdgeMethod;
        fprintf(stdout, "������ԭʼģ��·��\n");
        std::string path;
        std::cin>>path;
        if(path.empty()) return -1;
        shortestEdgeMethod.readObj(path);
        fprintf(stdout, "������ģ��Ŀ�궥������\n");
        int targetNums;
        scanf("%d", &targetNums);
        fprintf(stdout, "��ʼ��:\n");
        shortestEdgeMethod.simplifyMesh(targetNums);
        std::string pathOutput = path.substr(0, path.find_last_of('.')) + "_after.obj";
        shortestEdgeMethod.write_smf(pathOutput.c_str());
    }
    return 0;
}

