function [processors, U_bar] = TDTA(topology,C,T,U_bar)
%%%%%%%%% ����ǰdag������䴦����������U_bar���浱ǰ��������ʣ��������
m = length(U_bar);
V_i = length(topology);
%%%%%%%%% ��ʼ��
processors = zeros(1,V_i);

L = level(topology); %%%% �ֲ�

%%%%%�������
for i = 1:length(L)
    current_L = L{i}; %%%% 
    %%��һ�� 
    if  i == 1
        if length(current_L) > 1 %���source nodes
            mu = min(m, length(current_L)); %�� mu�͵���3 
            U_re = ones(1,mu); %%��������str�����ʣ��������
            index = zeros(1,mu); %%������¼U_re��Ӧ������������
            U_temp = U_bar; %[  0.8900    0.8800    0.9000    0.8700]
            for j = 1:mu
                max_U_temp = max(U_temp);
                max_U_temp = max_U_temp(1);
                t = find(U_bar == max_U_temp);
                index(j) = t(1);
                temp_remove = find(U_temp == U_bar(t(1)));
                temp_remove = temp_remove(1);
                U_temp(temp_remove) = [];
            end
            while ~isempty(current_L)
                %%%��Ϊ��
                max_c = max(C(current_L));
                max_c = max_c(1);  %%% max_c��������source nodes�� C�����Ǹ���ֵ
                max_index = current_L(C(current_L) == max_c);
                max_index = max_index(1); %%%% max_index����������C���������Ǹ�
                
                p_allocate = find(max(U_re) == U_re);
                p_allocate = p_allocate(1);
                
                processors(max_index) = p_allocate;
                %%%%%% ������ϣ��Ƴ���������,����������
                current_L(current_L == max_index) = [];
                U_re(p_allocate) = U_re(p_allocate) - C(max_index)/T;
                U_bar(p_allocate) = U_bar(p_allocate) - C(max_index)/T;
            end
        else%ֻ��һ��source node �����
            p_allocate = find(U_bar == max(U_bar));
            p_allocate = p_allocate(1);
            processors(current_L) = p_allocate;
            U_bar(p_allocate) = U_bar(p_allocate) - C(current_L)/T;
        end
    else  %%% ���ǵ�һ��
        %%%%%%%%%%%%%%%%%%%% �ҵ����е�str
        pro_str = current_L; % Ǳ�ڰ���str�ļ���                                        [1 1 1  5  5]
        pro_str(sum(topology(:,pro_str)) > 1) = []; %��ɾ���ж��pre��������  ����str = [2 3 4 12 19]
        pre_set = find(sum(topology(:,pro_str) == 1,2) > 1); %���е�str �ĸ��ڵ� pre_set = [1 5]
        while ~isempty(pre_set)
            current_pre = pre_set(1); %%%�鿴��ǰpre��suc�Ƿ���str, �ӵ�һ���鿴 current_pre = 1
            pre_set(1) = [];
            str = intersect(find(topology(current_pre,:)==1), pro_str); %% str = [2 3 4]
            if length(str) > 1 %������str
                mu = min(m, length(str)); %��ʼ���� mu = 3
                U_re = ones(1,mu); %%��������str�����ʣ��������
                index = zeros(1,mu); %%������¼U_re��Ӧ������������ [3 1 2]
                U_temp = U_bar;
                for j = 1:mu %�ҵ�����mu��������
                    max_U_temp = max(U_temp);
                    max_U_temp = max_U_temp(1);
                    t = find(U_bar == max_U_temp);
                    index(j) = t(1);
                    temp_remove = find(U_temp == U_bar(t(1)));
                    temp_remove = temp_remove(1);
                    U_temp(temp_remove) = [];
                end

                while ~isempty(str)
                    %%%��Ϊ��
                    max_c = max(C(str));
                    max_c = max_c(1);  %%% max_c��������source nodes�� C�����Ǹ���ֵ C2 = 82
                    max_index = str(C(str) == max_c);
                    max_index = max_index(1); %%%% max_index����������C���������Ǹ� index = 2
                    
                    p_allocate = find(max(U_re) == U_re);
                    p_allocate = p_allocate(1);

                    processors(max_index) = index(p_allocate);
                    %%%%%% ������ϣ��Ƴ���������,����������
                    str(str == max_index) = [];
                    U_re(p_allocate) = U_re(p_allocate) - C(max_index)/T;
                    U_bar(index(p_allocate)) = U_bar(index(p_allocate)) - C(max_index)/T;
                    current_L(current_L == max_index) = []; %%Ҳ��Ҫ��current_L����ɾ�������������
                end
            end
        end  %%%%�������е�Str����������
        
        while ~isempty(current_L)
            p_allocate = find(U_bar == max(U_bar));
            p_allocate = p_allocate(1);
            max_c = max(C(current_L));
            max_c = max_c(1);
            max_index = current_L(C(current_L) == max_c);
            max_index = max_index(1); %%%% max_index����������C���������Ǹ� index = 14
            processors(max_index) = p_allocate;
            U_bar(p_allocate) = U_bar(p_allocate) - C(max_index)/T;
            current_L(current_L == max_index) = [];
        end
    end
    
end



end