function pc = find_precedence_constraint(topology,a,b)
%%%%%% 该函数判断节点a是不是节点b的前继或间接前继子任务，是pc=1否则pc=0
x = length(topology); 

%%%%先判断a是否是b的直接前继子任务
if topology(a,b) == 1
    pc = 1;
    return
end

%%%否则，判断b有没有前继子任务
if sum(topology(:,b)) == 0
    pc = 0;
    return
else
    pre = find(topology(:,b) == 1);
    if sum(pre == a) %%a是b的直接前继子任务
        pc = 1;
        return
    else
        stack_b = pre; %%% 否则，压栈
    end
end

while ~isempty(stack_b) %%%栈不为空则表示b的间接前继子任务没有搜索完，就要继续搜索
    curr_sub = stack_b(1);%弹出栈顶元素并查看其前继子任务
    stack_b(1) = [];
    pre = find(topology(:,curr_sub) == 1); 
    if ~isempty(pre)
        if sum(pre == a) %%a是b的间接前继子任务
            pc = 1;
            return
        else
            stack_b = [stack_b; pre]; %%%%继续压栈
        end
    end
end

pc = 0; %%%已经搜索完了，都没有匹配到，

end
















