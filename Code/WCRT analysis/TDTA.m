function [processors, U_bar] = TDTA(topology,C,T,U_bar)
%%%%%%%%% 给当前dag任务分配处理器，其中U_bar储存当前处理器的剩余利用率
m = length(U_bar);
V_i = length(topology);
%%%%%%%%% 初始化
processors = zeros(1,V_i);

L = level(topology); %%%% 分层

%%%%%按层分配
for i = 1:length(L)
    current_L = L{i}; %%%% 
    %%第一层 
    if  i == 1
        if length(current_L) > 1 %多个source nodes
            mu = min(m, length(current_L)); %那 mu就等于3 
            U_re = ones(1,mu); %%用来进行str分配的剩余利用率
            index = zeros(1,mu); %%用来记录U_re对应处理器的索引
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
                %%%不为空
                max_c = max(C(current_L));
                max_c = max_c(1);  %%% max_c保存了在source nodes中 C最大的那个的值
                max_index = current_L(C(current_L) == max_c);
                max_index = max_index(1); %%%% max_index保存的是最大C子任务是那个
                
                p_allocate = find(max(U_re) == U_re);
                p_allocate = p_allocate(1);
                
                processors(max_index) = p_allocate;
                %%%%%% 分配完毕，移除该子任务,更新利用率
                current_L(current_L == max_index) = [];
                U_re(p_allocate) = U_re(p_allocate) - C(max_index)/T;
                U_bar(p_allocate) = U_bar(p_allocate) - C(max_index)/T;
            end
        else%只有一个source node 的情况
            p_allocate = find(U_bar == max(U_bar));
            p_allocate = p_allocate(1);
            processors(current_L) = p_allocate;
            U_bar(p_allocate) = U_bar(p_allocate) - C(current_L)/T;
        end
    else  %%% 不是第一层
        %%%%%%%%%%%%%%%%%%%% 找到所有的str
        pro_str = current_L; % 潜在包含str的集合                                        [1 1 1  5  5]
        pro_str(sum(topology(:,pro_str)) > 1) = []; %先删掉有多个pre的子任务  假设str = [2 3 4 12 19]
        pre_set = find(sum(topology(:,pro_str) == 1,2) > 1); %所有的str 的父节点 pre_set = [1 5]
        while ~isempty(pre_set)
            current_pre = pre_set(1); %%%查看当前pre的suc是否是str, 从第一个查看 current_pre = 1
            pre_set(1) = [];
            str = intersect(find(topology(current_pre,:)==1), pro_str); %% str = [2 3 4]
            if length(str) > 1 %构成了str
                mu = min(m, length(str)); %开始分配 mu = 3
                U_re = ones(1,mu); %%用来进行str分配的剩余利用率
                index = zeros(1,mu); %%用来记录U_re对应处理器的索引 [3 1 2]
                U_temp = U_bar;
                for j = 1:mu %找到最大的mu个处理器
                    max_U_temp = max(U_temp);
                    max_U_temp = max_U_temp(1);
                    t = find(U_bar == max_U_temp);
                    index(j) = t(1);
                    temp_remove = find(U_temp == U_bar(t(1)));
                    temp_remove = temp_remove(1);
                    U_temp(temp_remove) = [];
                end

                while ~isempty(str)
                    %%%不为空
                    max_c = max(C(str));
                    max_c = max_c(1);  %%% max_c保存了在source nodes中 C最大的那个的值 C2 = 82
                    max_index = str(C(str) == max_c);
                    max_index = max_index(1); %%%% max_index保存的是最大C子任务是那个 index = 2
                    
                    p_allocate = find(max(U_re) == U_re);
                    p_allocate = p_allocate(1);

                    processors(max_index) = index(p_allocate);
                    %%%%%% 分配完毕，移除该子任务,更新利用率
                    str(str == max_index) = [];
                    U_re(p_allocate) = U_re(p_allocate) - C(max_index)/T;
                    U_bar(index(p_allocate)) = U_bar(index(p_allocate)) - C(max_index)/T;
                    current_L(current_L == max_index) = []; %%也需要在current_L里面删除分配完的任务
                end
            end
        end  %%%%到此所有的Str都分配完了
        
        while ~isempty(current_L)
            p_allocate = find(U_bar == max(U_bar));
            p_allocate = p_allocate(1);
            max_c = max(C(current_L));
            max_c = max_c(1);
            max_index = current_L(C(current_L) == max_c);
            max_index = max_index(1); %%%% max_index保存的是最大C子任务是那个 index = 14
            processors(max_index) = p_allocate;
            U_bar(p_allocate) = U_bar(p_allocate) - C(max_index)/T;
            current_L(current_L == max_index) = [];
        end
    end
    
end



end