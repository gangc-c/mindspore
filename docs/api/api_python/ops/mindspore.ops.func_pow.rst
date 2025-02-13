mindspore.ops.pow
==================

.. py:function:: mindspore.ops.pow(input, exponent)

    计算 `input` 中每个元素的 `exponent` 次幂。

    .. math::

        out_{i} = input_{i} ^{ exponent_{i}}

    .. note::
        - 输入 `input` 和 `exponent` 遵循隐式类型转换规则，使数据类型保持一致。
        - 输入必须是两个Tensor，或一个Tensor和一个Scalar。
        - 当输入是两个Tensor时，它们的数据类型不能同时是bool，并保证其shape可以广播。
        - 当输入是一个Tensor和一个Scalar时，Scalar只能是一个常数。

    参数：
        - **input** (Union[Tensor, number.Number, bool]) - 第一个输入，是一个number.Number、bool值或数据类型为 `number <https://www.mindspore.cn/docs/zh-CN/master/api_python/mindspore/mindspore.dtype.html#mindspore.dtype>`_ 或 `bool_ <https://www.mindspore.cn/docs/zh-CN/master/api_python/mindspore/mindspore.dtype.html#mindspore.dtype>`_ 的Tensor。
        - **exponent** (Union[Tensor, number.Number, bool]) - 第二个输入，当第一个输入是Tensor时，第二个输入应该是一个number.Number或bool值，或数据类型为number或bool_的Tensor。当第一个输入是Scalar时，第二个输入必须是数据类型为number或bool_的Tensor。

    返回：
        Tensor，shape与广播后的shape相同，数据类型为两个输入中精度较高的类型。

    异常：
        - **TypeError** - `input` 和 `exponent` 不是Tensor、number.Number或bool。
        - **ValueError** - 当 `input` 和 `exponent` 都为Tensor时，它们的shape不相同。
