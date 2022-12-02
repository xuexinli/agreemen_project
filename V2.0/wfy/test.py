string = "sylnlxsrpyyacaolylwjeiswiuparlulsxrjisrsxrjsxwjriaesmmrwctjsxszasjwmpramhlxotxmarrjiaaqsoaxwasrpqaceiamnsxuiaesmmcaytrajpfamsaqasjsypxjiapjiacilxoiasrpyyacaorpnajisxueiswilyypcorlcalrpxypclwjsxusxlwwpcolxwajpisrsxrjsxwjriaesmmlwwabjsjaqaxpxjiarmsuijarjaqsoaxwajiapcsusxpynhjirsragbmlsxaosxjisrelhfacjclxoctrramm"
target = "ifamanisofferedafactwhichgoesagainsthisinstinctshewillscrutinizeitcloselyandunlesstheevidenceisoverwhelminghewillrefusetobelieveitifontheotherhandheisofferedsomethingwhichaffordsareasonforactinginaccordancetohisinstinctshewillacceptitevenontheslightestevidencetheoriginofmythsisexplainedinthiswaybertrandrussell"

result = {}

for i in range(len(string)):
    result[string[i]] = target[i]

aaa = []

for a in range(26):
    try :
        aaa.append([chr(97+a),result[chr(a+97)]])
    except:
        pass

print(aaa)

#正确[['a', 'e'], ['b', 'p'], ['c', 'r'], ['e', 'w'], ['f', 'b'], ['g', 'x'], ['h', 'y'], ['i', 'h'], ['j', 't'], ['l', 'a'], ['m', 'l'], ['n', 'm'], ['o', 'd'], ['p', 'o'], ['q', 'v'], ['r', 's'], ['s', 'i'], ['t', 'u'], ['u', 'g'], ['w', 'c'], ['x', 'n'], ['y', 'f'], ['z', 'z']]
#                                                                                                                                            


#有误[['a', 'e'], ['b', 'b'], ['c', 'l'], ['d', 'z'], ['e', 'w'], ['f', 'v'], ['g', 'k'], ['h', 'p'], ['i', 'n'], ['j', 'o'], ['k', 'x'], ['l', 's'], ['m', 'd'], ['n', 'g'], ['o', 'u'], ['p', 'h'], ['q', 'f'], ['r', 'a'], ['s', 't'], ['t', 'y'], ['u', 'm'], ['v', 'q'], ['w', 'r'], ['x', 'i'], ['y', 'c'], ['z', 'j']]

# 结果      a j l p s x r i c o m w t n e y u h b f q     g   z
# 原        e t a o i n s h r d l c u m w f g y p b v k j x q z 
# 计算      a s r j x i l p w m c y o u e q n t h b f g z k v d
#[['e', 0.12702], ['t', 0.09056], ['a', 0.08167], ['o', 0.07507], ['i', 0.06966], ['n', 0.06749], ['s', 0.06327], ['h', 0.06094], ['r', 0.05987], ['d', 0.04253], ['l', 0.04025], ['c', 0.02782], ['u', 0.02758], ['m', 0.02406], ['w', 0.0236], ['f', 0.02228], ['g', 0.02015], ['y', 0.01974], ['p', 0.01929], ['b', 0.01492], ['v', 0.00978], ['k', 0.00772], ['j', 0.00153], ['x', 0.0015], ['q', 0.00095], ['z', 0.00074]]
#[['a', 0.13183279742765272], ['s', 0.1157556270096463], ['r', 0.08360128617363344], ['j', 0.08038585209003216], ['x', 0.08038585209003216], ['i', 0.06752411575562701], ['l', 0.05787781350482315], ['p', 0.05466237942122187], ['w', 0.04823151125401929], ['m', 0.04823151125401929], ['c', 0.04501607717041801], ['y', 0.03858520900321544], ['o', 0.03215434083601286], ['u', 0.022508038585209004], ['e', 0.022508038585209004], ['q', 0.01607717041800643], ['n', 0.012861736334405145], ['t', 0.012861736334405145], ['h', 0.00964630225080386], ['b', 0.006430868167202572], ['f', 0.006430868167202572], ['g', 0.003215434083601286], ['z', 0.003215434083601286], ['k', 0.0], ['v', 0.0], ['d', 0.0]]
