#ifndef QUADRIC_H
#define QUADRIC_H
#include <map>
#include <deque>
#include <string>
#include <algorithm>
#include "Matrix.h"
#include <queue>
#include <set>

class Vertex {
public:
    double x;
    double y;
    double z;
    /* ������̱��У���ʾ�ö���Ķ���ֵ */
    double cost;
    /* ������̱��У���ʾ�ö���Ԥ���������� */
    int collapseTarget;
    /* �ö�����ڵ� */
    std::set<int> neighbors;
};

class Face {
public:
    int idVertex[3];//��������
    double plane[4];//�淽�̵�4������ax+by+cz+d=0
};

typedef std::map<int, Vertex> Vertices;
typedef std::deque<Face> Faces;
typedef std::pair<int, int> Pair;//v1��v2��������
typedef std::map<Pair, double> Errors;
//typedef std::deque<Vsplit> Vsplits;
typedef std::map<int, Matrix> Matrices;

class Quadrics {
private:
    Vertices vertices;// ���㼯��
    Faces faces;// �漯��
    Errors errors;// ����
    Matrices QMatrices;// Q���󼯺�
    double vertixError(double x, double y, double z, Matrix q) {
        return q[0]*x*x + 2*q[1]*x*y + 2*q[2]*x*z + 2*q[3]*x + q[5]*y*y
               + 2*q[6]*y*z + 2*q[7]*y + q[10]*z*z + 2*q[11]*z + q[15];
    }
public:
    Quadrics();
    ~Quadrics();
    //��ȡobj�ļ�����ʼ��vertices��faces
    void readObj(std::string filePath);
    //����obj�ļ�
    void parse(FILE* file);
    //��ʼ��ÿ�������QMatrix
    void initQMatrices(void);
    //ѡ�����кϷ���pair��ͬʱ��ʼ��errors
    void selectPairs();
    //��֪Vi��Vj�������pair�����Ժ��������V������deltV
    double calculateErrors(int idVi, int idVj, double* vx = 0, double* vy = 0, double* vz = 0);
    //ÿ�δ�errors��ѡ��delta��С��pair�����������������Ҹ����������
    void constructContract(int targetFaceNum);
    void write_smf(const char* filename);
    const int getFacesNum() const {
        return faces.size();
    }
    const int getVerticesNum() const {
        return vertices.size();
    }
    // ������Ժ��ԭʼ��������ƶ�
    double calculateSimilarity(Quadrics& originQuadrics);
    const Vertices getVertices() const {
        return vertices;
    }
    double minDisSquaredToAllPlance(Vertex vertex);
};
#endif