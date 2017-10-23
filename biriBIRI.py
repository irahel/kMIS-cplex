from random import randint

def biri(lastBiri):
    if lastBiri <= 1:
        lastBiri = 2
    biriPorcent = randint(1, 2* (lastBiri))
    if lastBiri % biriPorcent == 0:
        return (True, biriPorcent)
    else:
        return (False, 0)
    
def select(L, solu, biribi, rett = None)
    biriInd = 0;
    max_inter = 0
    ret = []
    res = []
    sE = rett
    biriRet = biribi
    for sub in L:
        if (not(sub in solu)) and (interc(sub, sE) > max_inter):
            max_inter = interc(sub, sE)
            ret = sub
        elif (not(sub in solu)) and (interc(sub, sE) == max_inter):
            biriTest = biri(biribi - biriInd)
            if biriTest[0]:
                biriRet = biriTest[1]
                res = ret
                ret = sub
            else:
                biriInd += 2
    
    return (ret, biriRet)

def interc(A, B):
    retu = 0
    for ele in A:
        if ele in B:
            retu += 1
    return retu

def intercc(A,B):
    ret = []
    for ele in A:
        if ele in B:
            ret += [ele]
    return ret
    
def first(L1):
    max = 0
    ret = None
    for sub in L1:
        if len(sub) > max:
            max = len(sub)
            ret = sub
    return ret

def execute(xazam):
    initialBiri = 2
    biribiri = initialBiri
    
    resposta = ['1','3','4']
    
    grafo = open('instancias/especiais/baixa/20 20 baixa.txt', 'r')
    demanda = open('instancias/especiais/baixa/demanda.txt', 'r')
    #Definicao da demanda
    k = int(demanda.read())
    
    demanda.close()
    #Definicao do grafo
    #Tamanho dos conjuntos
    #L = subconjuntos
    #R = elementos
    vertices = grafo.readlines()
    nL = int((vertices[0].split())[0])
    nR = int((vertices[0].split())[1])
    
    #print (vertices)
        
    #print ("k = " +str(k))
    #print ("L = " +str(nL))
    #print ("R = " +str(nR))
    
    L = []
    R = []
    for i in range(1,nL+1):
        S = []
        R += [i]
        for j in range(0,nR):
            if int((vertices[i].split())[j]) == 1:
                S += [j+1]
                #[int((vertices[i].split())[j]) == 1:]
        L += [S]
        #print (S)
    
    grafo.close()
    
    #print ("R = " +str(R))
    #print ("L = " +str(L))
    
    #Heuristica
    E = R
    solucao = []
    E = first(L)
    solucao += [first(L)]
    
    #print (solucao)
    #print (E)
    
    while len(solucao) < k:
        selected = select(L, solucao, biribiri,E)
        E = selected[0]
        solucao += [selected[0]]
        biribiri = selected[1]
    
    inter = intercc(solucao[0], solucao[1]) 
    if k > 2:
        for u in range(2,k):
            inter = intercc(inter, solucao[u])
    
    #print (resposta)
    print (len(inter))
    #print (inter)
    #print (solucao)    
        
    return (len(inter) == xazam, len(inter))


# Main
if __name__ == "__main__":
    resposta = 3
    acertos = 0
    
    for i in range(0,100):
        max = 0
        teste = execute(resposta)
        if teste[0]:
            acertos += 1
            if teste[1] > max:
                max = teste[1]
                
        
            
    print ("------acertos-----")
    print (acertos)
    
    print ("-----mais proximo----")
    print (teste[1])
    
