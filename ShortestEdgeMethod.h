#pragma once
#include "Quadric.h"
#include <float.h>

typedef std::map<int, double> VertexCost;
typedef VertexCost::value_type VertexCostVal;
class ShortestEdgeMethod {
private:
    /* ���㼯�� */
    Vertices vertices;
    /* �漯�� */
    Faces faces;
    /* ��¼ÿ�������cost */
    VertexCost vertexCost;
private:
    //����obj�ļ�
    void parse(FILE* file);
    // ��ʼ�����ж���cost
    void initCost();
    // ��ʼ������ڵ�
    void initNeighbors();
    // ����ĳһ�������cost��������Ӧ��
    float calcVertexCollapseCost(int vertexId);
    // �����߲���,����vertexId�����ոõ��¼��target��������
    void collapse(int vertexId);
    // ִ�������Ժ�����ϵ����
    void refreshFaces(int fromVertexId, int toVertexId);
    // �ж��������ж����Ƿ�Ϸ�
    bool isCorrectFace(Face face);
    // ˢ�¶����ڵ��cost
    void refreshNeighborsCost(std::set<int> neighbors);
    // ˢ���ڵ���ڵ�
    void refreshNeighbors(std::set<int> neighbors, int fromVertexId);
public:
    ShortestEdgeMethod(void);
    ~ShortestEdgeMethod(void);

    //��ȡobj�ļ�����ʼ��vertices��faces
    void readObj(std::string filePath);

    void write_smf(const char* filename);

    // ��
    void simplifyMesh(int targetVertices);
    double calculateEdgeLengthSqur(Vertex v0, Vertex v1);

};

