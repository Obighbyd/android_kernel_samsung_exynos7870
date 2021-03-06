/*
 * MELFAS MIP4 Touchkey
 *
 * Copyright (C) 2016 MELFAS Inc.
 *
 * mip4_sec.c : SEC command functions
 */

#include "mip4.h"

#if MIP_USE_CMD

/**
* Print chip firmware version
*/
static ssize_t mip4_tk_fw_version_panel(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct mip4_tk_info *info = dev_get_drvdata(dev);
	u8 data[255];
	int ret;
	u8 rbuf[16];

	memset(info->print_buf, 0, PAGE_SIZE);

	if (mip4_tk_get_fw_version(info, rbuf)) {
		dev_err(&info->client->dev, "%s [ERROR] mip4_tk_get_fw_version\n", __func__);
		sprintf(data, "NG\n");
		goto error;
	}

	dev_info(&info->client->dev, "%s - F/W Version : %02X.%02X/%02X.%02X/%02X.%02X/%02X.%02X\n", __func__, rbuf[0], rbuf[1], rbuf[2], rbuf[3], rbuf[4], rbuf[5], rbuf[6], rbuf[7]);
	sprintf(data, "0x%02X", rbuf[7]);

error:
	strcat(info->print_buf, data);

	ret = snprintf(buf, PAGE_SIZE, "%s", info->print_buf);
	return ret;
}

/**
* Print chip firmware version
*/
static ssize_t mip4_tk_cmd_fw_version_ic(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct mip4_tk_info *info = dev_get_drvdata(dev);
	u8 data[255];
	int ret;
	u8 rbuf[16];

	memset(info->print_buf, 0, PAGE_SIZE);

	if (mip4_tk_get_fw_version(info, rbuf)) {
		dev_err(&info->client->dev, "%s [ERROR] mip4_tk_get_fw_version\n", __func__);
		sprintf(data, "NG\n");
		goto error;
	}

	dev_info(&info->client->dev, "%s - F/W Version : %02X.%02X/%02X.%02X/%02X.%02X/%02X.%02X\n", __func__, rbuf[0], rbuf[1], rbuf[2], rbuf[3], rbuf[4], rbuf[5], rbuf[6], rbuf[7]);
	sprintf(data, "%02X.%02X/%02X.%02X/%02X.%02X/%02X.%02X\n", rbuf[0], rbuf[1], rbuf[2], rbuf[3], rbuf[4], rbuf[5], rbuf[6], rbuf[7]);

error:
	strcat(info->print_buf, data);

	ret = snprintf(buf, PAGE_SIZE, "%s", info->print_buf);
	return ret;
}

/**
* Print bin(file) firmware version
*/
static ssize_t mip4_tk_fw_version_phone(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct mip4_tk_info *info = dev_get_drvdata(dev);
	u8 data[255];
	int ret;
	u8 rbuf[16];

	if (mip4_tk_get_fw_version_from_bin(info, rbuf)) {
		dev_err(&info->client->dev, "%s [ERROR] mip_get_fw_version_from_bin\n", __func__);
		sprintf(data, "NG\n");
		goto error;
	}

	dev_info(&info->client->dev, "%s - BIN Version : %02X.%02X/%02X.%02X/%02X.%02X/%02X.%02X\n", __func__, rbuf[0], rbuf[1], rbuf[2], rbuf[3], rbuf[4], rbuf[5], rbuf[6], rbuf[7]);
	sprintf(data, "0x%02X", rbuf[7]);

error:
	ret = snprintf(buf, 255, "%s", data);
	return ret;
}

/**
* Print bin(file) firmware version
*/
static ssize_t mip4_tk_cmd_fw_version_bin(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct mip4_tk_info *info = dev_get_drvdata(dev);
	u8 data[255];
	int ret;
	u8 rbuf[16];

	if (mip4_tk_get_fw_version_from_bin(info, rbuf)) {
		dev_err(&info->client->dev, "%s [ERROR] mip_get_fw_version_from_bin\n", __func__);
		sprintf(data, "NG\n");
		goto error;
	}

	dev_info(&info->client->dev, "%s - BIN Version : %02X.%02X/%02X.%02X/%02X.%02X/%02X.%02X\n", __func__, rbuf[0], rbuf[1], rbuf[2], rbuf[3], rbuf[4], rbuf[5], rbuf[6], rbuf[7]);
	sprintf(data, "%02X.%02X/%02X.%02X/%02X.%02X/%02X.%02X\n", rbuf[0], rbuf[1], rbuf[2], rbuf[3], rbuf[4], rbuf[5], rbuf[6], rbuf[7]);

error:
	ret = snprintf(buf, 255, "%s", data);
	return ret;
}

/**
* Update firmware
*/
static ssize_t mip4_tk_cmd_fw_update(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct mip4_tk_info *info = i2c_get_clientdata(client);
	int result = 0;
	u8 data[255];
	int ret = 0;

	memset(info->print_buf, 0, PAGE_SIZE);

	dev_dbg(&info->client->dev, "%s [START]\n", __func__);

	ret = mip4_tk_fw_update_from_storage(info, info->fw_path_ext, true);

	switch (ret) {
	case fw_err_none:
		sprintf(data, "F/W update success.\n");
		break;
	case fw_err_uptodate:
		sprintf(data, "F/W is already up-to-date.\n");
		break;
	case fw_err_download:
		sprintf(data, "F/W update failed : Download error\n");
		break;
	case fw_err_file_type:
		sprintf(data, "F/W update failed : File type error\n");
		break;
	case fw_err_file_open:
		sprintf(data, "F/W update failed : File open error [%s]\n", info->fw_path_ext);
		break;
	case fw_err_file_read:
		sprintf(data, "F/W update failed : File read error\n");
		break;
	default:
		sprintf(data, "F/W update failed.\n");
		break;
	}

	dev_dbg(&info->client->dev, "%s [DONE]\n", __func__);

	strcat(info->print_buf, data);
	result = snprintf(buf, PAGE_SIZE, "%s", info->print_buf);
	return result;
}

/**
* Update firmware
*/
static ssize_t mip4_tk_fw_update(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct mip4_tk_info *info = i2c_get_clientdata(client);
	int result = 0;

	memset(info->print_buf, 0, PAGE_SIZE);

	switch(*buf) {
		info->firmware_state = 1;
	case 's':
	case 'S':
		result = mip4_tk_fw_update_from_kernel(info, true);
		if (result)
			info->firmware_state = 2;
		break;
	case 'i':
	case 'I':
		result = mip4_tk_fw_update_from_storage(info, info->fw_path_ext, true);
		if (result)
			info->firmware_state = 2;
		break;
	default:
		info->firmware_state = 2;
		goto exit;
	}

	info->firmware_state = 0;

exit:
	dev_info(&info->client->dev, "%s [DONE]\n", __func__);

	return count;
}

static ssize_t mip4_tk_cmd_fw_update_status(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct mip4_tk_info *info = i2c_get_clientdata(client);
	int count;

	dev_info(&info->client->dev, "%s : %d\n", __func__, info->firmware_state);

	if (info->firmware_state == 0)
		count = snprintf(buf, PAGE_SIZE, "PASS\n");
	else if (info->firmware_state == 1)
		count = snprintf(buf, PAGE_SIZE, "Downloading\n");
	else if (info->firmware_state == 2)
		count = snprintf(buf, PAGE_SIZE, "Fail\n");

	return count;
}

/**
* Sysfs print intensity
*/
static ssize_t mip4_tk_cmd_image(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct mip4_tk_info *info = dev_get_drvdata(dev);
	int key_idx = -1;
	int type = 0;
	int retry = 10;

	if (!strcmp(attr->attr.name, "touchkey_recent")) {
		key_idx = 0;
		type = MIP_IMG_TYPE_INTENSITY;
	} else if (!strcmp(attr->attr.name, "touchkey_recent_raw")) {
		key_idx = 0;
		type = MIP_IMG_TYPE_RAWDATA;
	} else if (!strcmp(attr->attr.name, "touchkey_back")) {
		key_idx = 1;
		type = MIP_IMG_TYPE_INTENSITY;
	} else if (!strcmp(attr->attr.name, "touchkey_back_raw")) {
		key_idx = 1;
		type = MIP_IMG_TYPE_RAWDATA;
	} else {
		dev_err(&info->client->dev, "%s [ERROR] Invalid attribute\n", __func__);
		goto error;
	}

	while (retry--) {
		if (info->test_busy == false) {
			break;
		}
		msleep(10);
	}
	if (retry <= 0) {
		dev_err(&info->client->dev, "%s [ERROR] skip\n", __func__);
		goto exit;
	}

	if (mip4_tk_get_image(info, type)) {
		dev_err(&info->client->dev, "%s [ERROR] mip_get_image\n", __func__);
		goto error;
	}

exit:
	dev_dbg(&info->client->dev, "%s - %s [%d]\n", __func__, attr->attr.name, info->image_buf[key_idx]);
	return snprintf(buf, PAGE_SIZE, "%d\n", info->image_buf[key_idx]);

error:
	dev_err(&info->client->dev, "%s [ERROR]\n", __func__);
	return snprintf(buf, PAGE_SIZE, "NG\n");
}

/**
* Sysfs print threshold
*/
static ssize_t mip4_tk_cmd_threshold(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct mip4_tk_info *info = dev_get_drvdata(dev);
	u8 wbuf[2];
	u8 rbuf[2];
	int threshold;

	wbuf[0] = MIP_R0_INFO;
	wbuf[1] = MIP_R1_INFO_CONTACT_THD_KEY;
	if (mip4_tk_i2c_read(info, wbuf, 2, rbuf, 1)) {
		goto error;
	}

	threshold = rbuf[0];

	dev_dbg(&info->client->dev, "%s - threshold [%d]\n", __func__, threshold);
	return snprintf(buf, PAGE_SIZE, "%d\n", threshold);

error:
	dev_err(&info->client->dev, "%s [ERROR]\n", __func__);
	return snprintf(buf, PAGE_SIZE, "NG\n");
}

#ifdef MIP_USE_LED
/**
* Store LED on/off
*/
static ssize_t mip4_tk_cmd_led_onoff_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct mip4_tk_info *info = dev_get_drvdata(dev);
	u8 wbuf[6];
	const char delimiters[] = ",\n";
	char string[count];
	char *stringp = string;
	char *token;
	int i, value, idx, bit;
	u8 values[4] = {0, };

	dev_dbg(&info->client->dev, "%s [START]\n", __func__);

	if (info->led_num <= 0) {
		dev_dbg(&info->client->dev, "%s - N/A\n", __func__);
		goto exit;
	}

	memset(string, 0x00, count);
	memcpy(string, buf, count);

	//Input format: "LED #0,LED #1,LED #2,..."
	//	Number of LEDs should be matched.

	for (i = 0; i < info->led_num; i++) {
		token = strsep(&stringp, delimiters);
		if (token == NULL) {
			dev_err(&info->client->dev, "%s [ERROR] LED number mismatch\n", __func__);
			goto error;
		} else {
			if (kstrtoint(token, 10, &value)) {
				dev_err(&info->client->dev, "%s [ERROR] wrong input value [%s]\n", __func__, token);
				goto error;
			}

			idx = i / 8;
			bit = i % 8;
			if (value == 1) {
				values[idx] |= (1 << bit);
			} else {
				values[idx] &= ~(1 << bit);
			}
		}
	}

	wbuf[0] = MIP_R0_LED;
	wbuf[1] = MIP_R1_LED_ON;
	wbuf[2] = values[0];
	wbuf[3] = values[1];
	wbuf[4] = values[2];
	wbuf[5] = values[3];
	if (mip4_tk_i2c_write(info, wbuf, 6)) {
		dev_err(&info->client->dev, "%s [ERROR] mip4_tk_i2c_write\n", __func__);
		goto error;
	}
	dev_dbg(&info->client->dev, "%s - wbuf 0x%02X 0x%02X 0x%02X 0x%02X\n", __func__, wbuf[2], wbuf[3], wbuf[4], wbuf[5]);

exit:
	dev_dbg(&info->client->dev, "%s [DONE]\n", __func__);
	return count;

error:
	dev_err(&info->client->dev, "%s [ERROR]\n", __func__);
	return count;
}

/**
* Show LED on/off
*/
static ssize_t mip4_tk_cmd_led_onoff_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct mip4_tk_info *info = dev_get_drvdata(dev);
	u8 data[255];
	int ret;
	u8 wbuf[2];
	u8 rbuf[4];
	int i, idx, bit;

	memset(info->print_buf, 0, PAGE_SIZE);

	dev_dbg(&info->client->dev, "%s [START]\n", __func__);

	if (info->led_num <= 0) {
		sprintf(data, "NA\n");
		goto exit;
	}

	wbuf[0] = MIP_R0_LED;
	wbuf[1] = MIP_R1_LED_ON;

	if (mip4_tk_i2c_read(info, wbuf, 2, rbuf, 4)) {
		dev_err(&info->client->dev, "%s [ERROR] mip4_tk_i2c_read\n", __func__);
		sprintf(data, "NG\n");
	} else {
		for (i = 0; i < info->led_num; i++) {
			idx = i / 8;
			bit = i % 8;
			if (i == 0) {
				sprintf(data, "%d", ((rbuf[idx] >> bit) & 0x01));
			} else {
				sprintf(data, ",%d", ((rbuf[idx] >> bit) & 0x01));
			}
			strcat(info->print_buf, data);
		}
	}
	sprintf(data, "\n");

exit:
	dev_dbg(&info->client->dev, "%s [DONE]\n", __func__);

	strcat(info->print_buf, data);
	ret = snprintf(buf, PAGE_SIZE, "%s", info->print_buf);
	return ret;
}

/**
* Store LED brightness
*/
static ssize_t mip4_tk_cmd_led_brightness_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct mip4_tk_info *info = dev_get_drvdata(dev);
	u8 wbuf[2 + MAX_LED_NUM];
	const char delimiters[] = ",\n";
	char string[count];
	char *stringp = string;
	char *token;
	int value, i;
	u8 values[MAX_LED_NUM];

	dev_dbg(&info->client->dev, "%s [START]\n", __func__);

	if (info->led_num <= 0) {
		dev_dbg(&info->client->dev, "%s - N/A\n", __func__);
		goto exit;
	}

	memset(string, 0x00, count);
	memcpy(string, buf, count);

	//Input format: "LED #0,LED #1,LED #2,..."
	//	Number of LEDs should be matched.

	for (i = 0; i < info->led_num; i++) {
		token = strsep(&stringp, delimiters);
		if (token == NULL) {
			dev_err(&info->client->dev, "%s [ERROR] LED number mismatch\n", __func__);
			goto error;
		} else {
			if (kstrtoint(token, 10, &value)) {
				dev_err(&info->client->dev, "%s [ERROR] wrong input value\n", __func__);
				goto error;
			}
			values[i] = (u8)value;
		}
	}

	wbuf[0] = MIP_R0_LED;
	wbuf[1] = MIP_R1_LED_BRIGHTNESS;
	memcpy(&wbuf[2], values, info->led_num);
	if (mip4_tk_i2c_write(info, wbuf, (2 + info->led_num))) {
		dev_err(&info->client->dev, "%s [ERROR] mip4_tk_i2c_write\n", __func__);
		goto error;
	}

exit:
	dev_dbg(&info->client->dev, "%s [DONE]\n", __func__);
	return count;

error:
	dev_err(&info->client->dev, "%s [ERROR]\n", __func__);
	return count;
}

/**
* Show LED brightness
*/
static ssize_t mip4_tk_cmd_led_brightness_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct mip4_tk_info *info = dev_get_drvdata(dev);
	u8 data[255];
	int ret, i;
	u8 wbuf[2];
	u8 rbuf[MAX_LED_NUM];

	memset(info->print_buf, 0, PAGE_SIZE);

	dev_dbg(&info->client->dev, "%s [START]\n", __func__);

	if (info->led_num <= 0) {
		sprintf(data, "NA\n");
		goto exit;
	}

	wbuf[0] = MIP_R0_LED;
	wbuf[1] = MIP_R1_LED_BRIGHTNESS;
	if (mip4_tk_i2c_read(info, wbuf, 2, rbuf, info->led_num)) {
		dev_err(&info->client->dev, "%s [ERROR] mip4_tk_i2c_read\n", __func__);
		sprintf(data, "NG\n");
	} else {
		for (i = 0; i < info->led_num; i++) {
			if (i == 0) {
				sprintf(data, "%d", rbuf[i]);
			} else {
				sprintf(data, ",%d", rbuf[i]);
			}
			strcat(info->print_buf, data);
		}
		sprintf(data, "\n");
	}

exit:
	dev_dbg(&info->client->dev, "%s [DONE]\n", __func__);

	strcat(info->print_buf, data);
	ret = snprintf(buf, PAGE_SIZE, "%s", info->print_buf);
	return ret;
}
#endif // MIP_USE_LED

static DEVICE_ATTR(touchkey_firm_version_phone, S_IRUGO, mip4_tk_fw_version_phone, NULL);
static DEVICE_ATTR(touchkey_firm_version_panel, S_IRUGO, mip4_tk_fw_version_panel, NULL);
static DEVICE_ATTR(fw_version_ic, S_IRUGO, mip4_tk_cmd_fw_version_ic, NULL);
static DEVICE_ATTR(fw_version_bin, S_IRUGO, mip4_tk_cmd_fw_version_bin, NULL);
static DEVICE_ATTR(fw_update, S_IRUGO, mip4_tk_cmd_fw_update, NULL);
static DEVICE_ATTR(touchkey_firm_update, S_IWUSR | S_IWGRP, NULL, mip4_tk_fw_update);
static DEVICE_ATTR(touchkey_firm_update_status, S_IRUGO, mip4_tk_cmd_fw_update_status, NULL);
static DEVICE_ATTR(touchkey_recent, S_IRUGO, mip4_tk_cmd_image, NULL);
static DEVICE_ATTR(touchkey_recent_raw, S_IRUGO, mip4_tk_cmd_image, NULL);
static DEVICE_ATTR(touchkey_back, S_IRUGO, mip4_tk_cmd_image, NULL);
static DEVICE_ATTR(touchkey_back_raw, S_IRUGO, mip4_tk_cmd_image, NULL);
static DEVICE_ATTR(touchkey_threshold, S_IRUGO, mip4_tk_cmd_threshold, NULL);
#ifdef MIP_USE_LED
static DEVICE_ATTR(led_onoff, S_IRUGO | S_IWUSR | S_IWGRP, mip4_tk_cmd_led_onoff_show, mip4_tk_cmd_led_onoff_store);
static DEVICE_ATTR(led_brightness, S_IRUGO | S_IWUSR | S_IWGRP, mip4_tk_cmd_led_brightness_show, mip4_tk_cmd_led_brightness_store);
#endif //MIP_USE_LED

/**
* Sysfs - touchkey attr info
*/
static struct attribute *mip_cmd_key_attr[] = {
	&dev_attr_fw_version_ic.attr,
	&dev_attr_fw_version_bin.attr,	
	&dev_attr_touchkey_firm_version_phone.attr,
	&dev_attr_touchkey_firm_version_panel.attr,
	&dev_attr_touchkey_firm_update.attr,
	&dev_attr_touchkey_firm_update_status.attr,
	&dev_attr_fw_update.attr,
	&dev_attr_touchkey_recent.attr,
	&dev_attr_touchkey_recent_raw.attr,
	&dev_attr_touchkey_back.attr,
	&dev_attr_touchkey_back_raw.attr,
	&dev_attr_touchkey_threshold.attr,
#ifdef MIP_USE_LED
	&dev_attr_led_onoff.attr,
	&dev_attr_led_brightness.attr,
#endif // MIP_USE_LED		
	NULL,
};

/**
* Sysfs - touchkey attr group info
*/
static const struct attribute_group mip_cmd_key_attr_group = {
	.attrs = mip_cmd_key_attr,
};

//extern struct class *sec_class;

/**
* Create sysfs command functions
*/
int mip4_tk_sysfs_cmd_create(struct mip4_tk_info *info)
{
	struct i2c_client *client = info->client;

	if (info->print_buf == NULL) {
		info->print_buf = kzalloc(sizeof(u8) * PAGE_SIZE, GFP_KERNEL);
	}
	if (info->image_buf == NULL) {
		info->image_buf = kzalloc(sizeof(int) * PAGE_SIZE, GFP_KERNEL);
	}

	info->key_dev = sec_device_create(info, "sec_touchkey");
	if (sysfs_create_group(&info->key_dev->kobj, &mip_cmd_key_attr_group)) {
		dev_err(&client->dev, "%s [ERROR] sysfs_create_group\n", __func__);
		return -EAGAIN;
	}

	return 0;
}

/**
* Remove sysfs command functions
*/
void mip4_tk_sysfs_cmd_remove(struct mip4_tk_info *info)
{
//	device_destroy(sec_class,  0);
	return;
}

#endif

