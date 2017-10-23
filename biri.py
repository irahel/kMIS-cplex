def select(L, solu):
    max_inter = 0
    ret = None
    for sub in L:
        #print (sub)
        if not(sub in solu):
            if len(sub) > max_inter:
                max_inter = len(sub)
                ret = sub
    return ret


    
#Heuristica gulosa do artigo
# Main
if __name__ == "__main__":
    #Definicao da demanda
    k = 2
    
    #Definicao do grafo
    #Tamanho dos conjuntos
    #L = subconjuntos
    #R = elementos
    nL = 4
    nR = 5
    
    #Elementos
    e1 = "e1"
    e2 = "e2"
    e3 = "e3"
    e4 = "e4"
    e5 = "e5"
    
    #subconjuntos
    s1 = [e2, e3]
    s2 = [e1, e2, e3]
    s3 = [e1, e2, e3, e4, e5]
    s4 = [e2, e3, e5]
    
    #Grafo
    L = [s1, s2, s3, s4]
    R = [e1, e2, e3, e4, e5]
    
    #print (L)
    #Heuristica
    E = R
    solucao = []
    
    while len(solucao) < k:
        selected = select(L, solucao)
        E = [selected]
        solucao += [selected]
    
    print (solucao)
        
