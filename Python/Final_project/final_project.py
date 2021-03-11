# -*- coding: utf-8 -*-
import pandas as pd
import tkinter as tk
from sklearn.linear_model import LinearRegression
import matplotlib.pyplot as plt
import numpy as np
import random
import statistics

#Investment_Opportunity
Invest_op = pd.read_csv('Investment_Opportunity.csv')
LQD_avg=Invest_op['Ret LQD (%)'].sum()/(len(Invest_op)-1)
SPY_avg=Invest_op['Ret SPY (%)'].sum()/(len(Invest_op)-1)
LQD_stdev=statistics.stdev(list(Invest_op['Ret LQD (%)'][1:]))
SPY_stdev=statistics.stdev(list(Invest_op['Ret SPY (%)'][1:]))
Corrcoef=np.corrcoef(list(Invest_op['Ret LQD (%)'][1:]),list(Invest_op['Ret SPY (%)'][1:]))[0][1]
Cov=np.cov(list(Invest_op['Ret LQD (%)'][1:]),list(Invest_op['Ret SPY (%)'][1:]))[0][1]

#畫The Investment Opportunity Set
Mean=[]
SD=[]
SD_iv=[]
SD_v=[]
for i in range(11):
    m=i/10
    n=1-i/10
    Mean.insert(i,m*LQD_avg+n*SPY_avg)
    SD.insert(i, np.sqrt((m*LQD_stdev)**2+(n*SPY_stdev)**2+2*m*LQD_stdev*n*SPY_stdev*Corrcoef))
    SD_iv.insert(i, np.sqrt((m*LQD_stdev)**2+(n*SPY_stdev)**2+2*m*LQD_stdev*n*SPY_stdev))
    SD_v.insert(i, np.sqrt((m*LQD_stdev)**2+(n*SPY_stdev)**2+2*m*LQD_stdev*n*SPY_stdev*-1))

fig1, ax1 = plt.subplots()
ax1.plot(SD, Mean, marker='o')
ax1.plot(SD_iv, Mean, marker='o')
ax1.plot(SD_v, Mean, marker='o')
plt.title('The Investment Opportunity Set')
plt.xlabel('Standard Deviation(%)')
plt.ylabel('Expected Return(%)')
fig1.savefig("The Investment Opportunity Set.png")

#random
np.random.seed(1)
random.seed(1)

#讀檔
data = pd.read_csv('S&P_500_5y.csv')

#資料處理
random_list=np.random.permutation(data.index)
r_data=data.loc[random_list]

#此部分是做開盤跟收盤的線性回歸預測
#把資料分成train的跟用來test的
train_row=int(len(r_data)*0.7)


train_data=r_data[:train_row]
test_data=r_data[train_row:]

#線性回歸
regressor = LinearRegression()
regressor.fit(train_data['Open'].values.reshape(-1,1), train_data['Close'].values.reshape(-1,1))
predict_reg=regressor.predict(test_data['Open'].values.reshape(-1,1))

#mean-square error 誤差
MSE=sum((test_data['Close'].values.reshape(-1,1)-predict_reg)**2)/len(predict_reg)
print("mean-square error: "+str(MSE[0]))

fig2, ax2 = plt.subplots()
ax2.scatter(test_data['Open'], test_data['Close'])
ax2.plot(test_data['Open'], predict_reg,"r")
plt.title('S&P500_openprice_closeprice_LinearRegression_predict')
plt.xlabel('Open Price')
plt.ylabel('Close Price')
fig2.savefig("S&P500_openprice_closeprice_LinearRegression_predict.png")


#GUI的部分
window = tk.Tk()
window.title("Stock Predictor")

frame1=tk.Frame(window)
frame2=tk.Frame(window)
var=tk.StringVar()
label_1=tk.Label(frame1, text="TWGF 股票趨勢預測器", font = 'Helvetica -15 bold').pack()
label_2=tk.Label(frame1, text="請輸入指定關鍵字(股票名稱)").pack()
label_3=tk.Label(frame2)
label_3.pack()
e_1 = tk.Entry(frame1, textvariable=var).pack()

def button_onclick():       
    vtext=var.get()
    
    #讀入跟輸入一樣的股票公司
    data_company = pd.read_csv(vtext+'.csv')
    r_data_company=data_company.loc[random_list]
    train_data_company=r_data_company[:train_row]
    test_data_company=r_data_company[train_row:]

    #此部分是做公司每日收盤價和SP500收盤價的線性回歸預測
    regressor = LinearRegression()
    regressor.fit(train_data['Close'].values.reshape(-1,1), train_data_company['Close'].values.reshape(-1,1))
    predict_reg=regressor.predict(test_data['Close'].values.reshape(-1,1))
    
    #mean-square error 誤差
    MSE=sum((test_data_company['Close'].values.reshape(-1,1)-predict_reg)**2)/len(predict_reg)
    print("mean-square error: "+str(MSE[0]))
    
    fig3, ax3 = plt.subplots()
    ax3.scatter(test_data['Close'], test_data_company['Close'])
    ax3.plot(test_data['Close'], predict_reg,"r")
    plt.title('Company_and_S&P500_closeprice_LinearRegression_predict')
    plt.xlabel('S&P500 Close Price')
    plt.ylabel('Company Close Price')
    fig3.savefig(vtext+"_closeprice_and_S&P500_closeprice_LinearRegression_predict.png")
    plt.show()
    
    photo=tk.PhotoImage(file=vtext+"_closeprice_and_S&P500_closeprice_LinearRegression_predict.png")
    
    label_3.configure(image=photo)
    label_3.image = photo
    
    
    
button1 = tk.Button(frame1, text='送出',command=button_onclick).pack()
frame1.pack(padx=10,pady=10)
frame2.pack(padx=10,pady=10)
window.mainloop()
