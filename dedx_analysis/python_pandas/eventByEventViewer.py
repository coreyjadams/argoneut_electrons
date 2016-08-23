import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Button

from dataFrameHandle import dataFrameHandle


f, (ax1, ax2) = plt.subplots(1, 2, sharey=True, figsize=[15, 7.5])
plt.subplots_adjust(bottom=0.2)
df = dataFrameHandle("anatrees/electrons_anatree.root", "electron")
# print l

def plotData(row):

    # print type(row)
    # print row
    # return

    electrons = [2.3]*len(row['c_dist_from_start'])
    photons = [4.6]*len(row['c_dist_from_start'])
    # plt.close('all')
    # Two subplots, unpack the axes array immediately
    f.suptitle(
        "dE/dx, Run {r}, Event {e}".format(r=row['run'], e=row.event))
    ax1.plot(row['c_dist_from_start'], row[
             'c_charge_dedx_box'], marker='+', markersize=20,label="box")
    ax1.plot(row['c_dist_from_start'], row[
             'c_charge_dedx_const'], marker='x', markersize=20,label="const")
    ax1.plot(row['c_dist_from_start'], electrons, linestyle='--')
    ax1.plot(row['c_dist_from_start'], photons, linestyle='--')
    plt.ylabel("dE/dx [MeV/cm]")
    plt.legend()
    f.text(0.01, 0.65, "modmean: {:.2}".format(
        row['c_charge_dedx_const_mean_no_outliers_0']))
    f.text(0.01, 0.6, "Med: {:.2}".format(row['c_charge_dedx_const_median_0']))
    f.text(0.01, 0.55, "LMA: {:.2}".format(row['c_charge_dedx_const_LMA']))
    text = "dE/dx: {:.2} +/- {:.2n}%".format(row["c_charge_dedx_const_meta_0"], 100.*row["c_charge_dedx_const_meta_0_err"]/row["c_charge_dedx_const_meta_0"])
    ax1.set_title('Collection Plane, ' + text)
    ax1.grid(True)


    # ax2.scatter(x, y)
    ax2.plot(row['i_dist_from_start'], row[
             'i_charge_dedx_box'], marker='+', markersize=20,label="box")
    ax2.plot(row['i_dist_from_start'], row[
             'i_charge_dedx_const'], marker='x', markersize=20,label="const")
    electrons = [2.3]*len(row['i_dist_from_start'])
    photons = [4.6]*len(row['i_dist_from_start'])
    ax2.plot(row['i_dist_from_start'], electrons, linestyle='--')
    ax2.plot(row['i_dist_from_start'], photons, linestyle='--')
    ax2.grid(True)
    plt.legend()

    f.text(0.91, 0.65, "2: {:.2}".format(
        row['i_charge_dedx_const_mean_no_outliers_0']))
    f.text(0.91, 0.6, "3: {:.2}".format(row['i_charge_dedx_const_median_0']))
    f.text(0.91, 0.55, "4: {:.2}".format(row['i_charge_dedx_const_LMA']))
    # Add some text to the plot to display the dE/dx calculated:
    text = "dE/dx: {:.2} +/- {:.2n}%".format(row["i_charge_dedx_const_meta_0"], 100.*row["i_charge_dedx_const_meta_0_err"]/row["i_charge_dedx_const_meta_0"])
    ax2.set_title('Induction Plane, ' + text)

    # Compute a mean and rms of some of the values of dE/dx:
    dEdx_v = []
    dEdx_v.append(row['c_charge_dedx_const_median_0'])
    dEdx_v.append(row['c_charge_dedx_const_LMA'])
    dEdx_v.append(row['c_charge_dedx_const_mean_no_outliers_0'])
    dEdx_v.append(row['i_charge_dedx_const_median_0'])
    dEdx_v.append(row['i_charge_dedx_const_LMA'])
    dEdx_v.append(row['i_charge_dedx_const_mean_no_outliers_0'])

    # print dEdx_v

    mean_dedx = np.mean(dEdx_v)
    rms_dedx = np.std(dEdx_v)
    percent = (100.*rms_dedx)/mean_dedx
    f.text(0.45, 0.04, "Average: {a:.2} +\- {b:.2} ({c:.2n}%)".format(a=mean_dedx,b=rms_dedx,c=percent))

    x1, x2, y1, y2 = plt.axis()
    plt.axis((x1, x2, 0, 10))



# This cute little class lets me access the data and update the plots
class Event(object):
    ind = 0
    max_index = 0


    def setDataFrame(self, df):
      self._df = df
      self.max_index = self._df.n_events() - 1

    def next(self, event):
        self.ind += 1
        if self.ind >= self.max_index:
          self.ind = 0
        # ydata = np.sin(2*np.pi*freqs[i]*t)
        # l.set_ydata(ydata)
        self.updatePlot()
        plt.draw()

    def prev(self, event):
        self.ind -= 1
        if self.ind < 0:
          self.ind = self.max_index

        # ydata = np.sin(2*np.pi*freqs[i]*t)
        # l.set_ydata(ydata)
        self.updatePlot()
        plt.draw()

    def updatePlot(self):
      row = self._df.get_row(self.ind)
      f.suptitle("dE/dx, Run {r}, Event {e}".format(r=row['run'], e=row.event))
      ax1.clear()

      for txt in f.texts:
        txt.remove()
        # f.remove(txt)

      electrons = [2.3]*len(row['c_dist_from_start'])
      photons = [4.6]*len(row['c_dist_from_start'])
      ax1.plot(row['c_dist_from_start'], row[
             'c_charge_dedx_box'], marker='+', markersize=20,label="box")
      ax1.plot(row['c_dist_from_start'], row[
             'c_charge_dedx_const'], marker='x', markersize=20,label="const")
      ax1.plot(row['c_dist_from_start'], electrons, linestyle='--')
      ax1.plot(row['c_dist_from_start'], photons, linestyle='--')
      plt.ylabel("dE/dx [MeV/cm]")
      plt.legend()
      f.text(0.01, 0.65, "modmean: {:.2}".format(
          row['c_charge_dedx_const_mean_no_outliers_0']))
      f.text(0.01, 0.6, "Med: {:.2}".format(row['c_charge_dedx_const_median_0']))
      f.text(0.01, 0.55, "LMA: {:.2}".format(row['c_charge_dedx_const_LMA']))
      text = "dE/dx: {:.2} +/- {:.2n}%".format(row["c_charge_dedx_const_meta_0"], 100.*row["c_charge_dedx_const_meta_0_err"]/row["c_charge_dedx_const_meta_0"])
      ax1.set_title('Collection Plane, ' + text)
      ax1.grid(True)

      ax2.clear()
      ax2.plot(row['i_dist_from_start'], row[
               'i_charge_dedx_box'], marker='+', markersize=20,label="box")
      ax2.plot(row['i_dist_from_start'], row[
               'i_charge_dedx_const'], marker='x', markersize=20,label="const")
      electrons = [2.3]*len(row['i_dist_from_start'])
      photons = [4.6]*len(row['i_dist_from_start'])
      ax2.plot(row['i_dist_from_start'], electrons, linestyle='--')
      ax2.plot(row['i_dist_from_start'], photons, linestyle='--')
      ax2.grid(True)
      plt.legend()

      f.text(0.91, 0.65, "2: {:.2}".format(
          row['i_charge_dedx_const_mean_no_outliers_0']))
      f.text(0.91, 0.6, "3: {:.2}".format(row['i_charge_dedx_const_median_0']))
      f.text(0.91, 0.55, "4: {:.2}".format(row['i_charge_dedx_const_LMA']))
      # Add some text to the plot to display the dE/dx calculated:
      text = "dE/dx: {:.2} +/- {:.2n}%".format(row["i_charge_dedx_const_meta_0"], 100.*row["i_charge_dedx_const_meta_0_err"]/row["i_charge_dedx_const_meta_0"])
      ax2.set_title('Induction Plane, ' + text)

      # Compute a mean and rms of some of the values of dE/dx:
      dEdx_v = []
      dEdx_v.append(row['c_charge_dedx_const_median_0'])
      dEdx_v.append(row['c_charge_dedx_const_LMA'])
      dEdx_v.append(row['c_charge_dedx_const_mean_no_outliers_0'])
      dEdx_v.append(row['i_charge_dedx_const_median_0'])
      dEdx_v.append(row['i_charge_dedx_const_LMA'])
      dEdx_v.append(row['i_charge_dedx_const_mean_no_outliers_0'])

      # print dEdx_v

      mean_dedx = np.mean(dEdx_v)
      rms_dedx = np.std(dEdx_v)
      percent = (100.*rms_dedx)/mean_dedx
      f.text(0.45, 0.04, "Average: {a:.2} +\- {b:.2} ({c:.2n}%)".format(a=mean_dedx,b=rms_dedx,c=percent))

      x1, x2, y1, y2 = plt.axis()
      plt.axis((x1, x2, 0, 10))
# callback = Index()


# plt.show()

plotData(df.get_row(0))

# Initialize a dataframe:

callback = Event()
callback.setDataFrame(df)
axprev = plt.axes([0.7, 0.05, 0.1, 0.075])
axnext = plt.axes([0.81, 0.05, 0.1, 0.075])
bnext = Button(axnext, 'Next')
bnext.on_clicked(callback.next)
bprev = Button(axprev, 'Previous')
bprev.on_clicked(callback.prev)

plt.show()
